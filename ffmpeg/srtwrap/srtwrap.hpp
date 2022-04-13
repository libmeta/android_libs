#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <numeric>

#include <srt/srt.h>

#include "task/task.hpp"
#include "time/time_utils.hpp"
#include "xlog_common.hpp"

using namespace xlab;

class SRTWrap : public XLogLevelBase {
public:
    explicit SRTWrap(SRTSOCKET sock)
        : sock(sock)
        , XLogLevelBase()
    {
    }

    ~SRTWrap()
    {
    }

    bool updateVideoEncodeBitate(int64_t& current_vencode_bitrate, const int64_t video_stream_bitrate)
    {
        static constexpr double INCR_RATION = 1.08;
        static constexpr double DECR_RATION = 0.85;

        int64_t tmp_vencode_bitrate = current_vencode_bitrate;
        if (update_vencode_bitrate_task.run([this, &current_vencode_bitrate, &video_stream_bitrate, &tmp_vencode_bitrate] {
                if (congestion_state == STATE_INCR) { // not adjust bitrate for bandwith is enough
                    if (current_vencode_bitrate < video_stream_bitrate * 1.3) {
                        tmp_vencode_bitrate = current_vencode_bitrate * INCR_RATION;
                        dlog("increase");
                    }
                } else if (congestion_state == STATE_DECR) { // adjust bitrate to estimate_bandwith cbr
                    if (current_vencode_bitrate > video_stream_bitrate * 0.4) {
                        tmp_vencode_bitrate = current_vencode_bitrate * DECR_RATION;
                        dlog("decrease");
                    }
                }
            })) {
            update_vencode_bitrate_task.reset();
            if (tmp_vencode_bitrate != current_vencode_bitrate) {
                current_vencode_bitrate = tmp_vencode_bitrate;
                return true;
            } else {
                dlog("keep");
            }
        }

        return false;
    }

    void congestionCtrl()
    {
        if (srt_congestion_ctrl_task.run([this] {
                SRT_TRACEBSTATS perf;
                const auto srt_bstats_result = srt_bstats(sock, &perf, 1);
                if (srt_bstats_result != 0) {
                    return;
                }

                const int64_t inflight = perf.pktFlightSize * 188 * 7;
                const int64_t bw_bitrate = perf.mbpsSendRate * 1000 * 1000; //_last_send_bytes*8*1000/diff_t;
                const int rtt = std::max((int)(perf.msRTT), RTT_MIN);

                updateRTT(rtt);
                updateMaxBW(bw_bitrate);
                congestion_state = srtBitrateGetState(inflight);

                dlog("congestion ctrl({}) return {}, rtt:{}, inflight:{}, bw_bitrate:{}",
                    sock, srt_bstats_result, rtt, inflight, bw_bitrate);
            })) {
            srt_congestion_ctrl_task.reset();
        }
    }

    SRT_SOCKSTATUS getSockstate() const
    {
        return srt_getsockstate(sock);
    }

    bool getSndBuffer(size_t* bytes, size_t* blocks)
    {
        const int st = srt_getsndbuffer(sock, blocks, bytes);
        if (st == SRT_ERROR) {
            return false;
        }

        return true;
    }

    const std::string& getLastError() const
    {
        return std::string(srt_getlasterror_str());
    }

private:
    void updateRTT(int rtt)
    {
        rtt_array.push_back(rtt = std::max(rtt, RTT_MIN));
        if ((rtt_ready = rtt_array.size() >= RTT_LIST_MAX)) {
            rtt_min = *std::min_element(rtt_array.begin(), rtt_array.end());
            rtt_array.clear();
            dlog("update rtt={}, input rtt:{}", rtt_min, rtt);
        }
    }

    // update current max bandwith(bits/s)
    void updateMaxBW(uint64_t maxbw)
    {
        bw_array.push_back(maxbw);
        if ((bw_ready = bw_array.size() >= BW_LIST_MAX)) {
            avg_bw = std::accumulate(bw_array.begin(), bw_array.end(), 0) / bw_array.size();
            bw_max = *std::max_element(bw_array.begin(), bw_array.end());
            bw_max = (bw_max + avg_bw) * 1.2 / 2;
            bw_array.clear();
        }
    }

    // get the state whether adjust the encoder bitrate.
    int srtBitrateGetState(int inflight)
    {
        static constexpr int64_t INCR = 1316 * 8 * 3;
        static constexpr int64_t DECR = -1316 * 8 * 6;

        if (!rtt_ready || !bw_ready) {
            return STATE_KEEP;
        }

        const double bdp = bw_max * rtt_min / 1000;
        inflight = inflight * 8;
        const int64_t current_state = cwnd_gain * bdp - inflight;
        state_array.push_back(current_state);
        if (!(state_ready = state_array.size() >= STATE_LIST_MAX)) {
            return STATE_KEEP;
        }

        const int64_t final_state = std::accumulate(state_array.begin(), state_array.end(), 0l);
        dlog("get congestion inflight:{}, _bw_max:{}, _bw_avg:{}, _rtt_min:{}, bdp:{}, final_state:{}, current_state:{}",
            inflight, bw_max, avg_bw, rtt_min, bdp, final_state, current_state);

        if (final_state > (INCR)) {
            return STATE_INCR;
        } else if (final_state < (DECR - 1)) {
            return STATE_DECR;
        }

        return STATE_KEEP;
    }

private:
    static constexpr auto VIDEO_UPDATE_INTERVAL = 500ms;

    static constexpr auto SRT_CHECK_INTERVAL = 300ms;
    static constexpr int RTT_LIST_MAX = 6;
    static constexpr int BW_LIST_MAX = 6;
    static constexpr int STATE_LIST_MAX = 6;
    static constexpr double CWND_GAIN_DEF = 1.3;
    static constexpr int RTT_INIT = 100;
    static constexpr int RTT_MIN = 35;

    enum {
        STATE_INCR = 0x01,
        STATE_DECR = 0x02,
        STATE_KEEP = 0x03,
    };

private:
    SRTSOCKET sock;

    std::vector<int> rtt_array;
    int rtt_min = RTT_INIT; // ms
    bool rtt_ready = false;

    std::vector<uint64_t> bw_array; // bits/s
    uint64_t bw_max = 0; // bits/s, max srt bandwidth
    uint64_t avg_bw = 0; // bits/s, avg srt bandwidth
    bool bw_ready = false;

    std::vector<int64_t> state_array;
    bool state_ready = false;

    double cwnd_gain = CWND_GAIN_DEF;
    std::atomic<int> congestion_state = STATE_KEEP;

    xlab::Task update_vencode_bitrate_task { 1, VIDEO_UPDATE_INTERVAL };
    xlab::Task srt_congestion_ctrl_task { 1, SRT_CHECK_INTERVAL };
};
