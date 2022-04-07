#pragma once

#include <algorithm>
#include <array>
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
    {
    }

    ~SRTWrap()
    {
    }

    void updateRTT(int rtt)
    {
        s_rtt_array.push_back(rtt = std::max(rtt, RTT_MIN));
        if ((s_rtt_ready = s_rtt_array.size() >= RTT_LIST_MAX)) {
            s_rtt_min = *std::min_element(s_rtt_array.begin(), s_rtt_array.end());
            s_rtt_array.clear();
            llog("update rtt={}, input rtt:{}", s_rtt_min, rtt);
        }
    }

    // update current max bandwith(bits/s)
    void updateMaxBW(uint64_t maxbw)
    {
        s_bw_array.push_back(maxbw);
        if ((s_bw_ready = s_bw_array.size() >= BW_LIST_MAX)) {
            s_avg_bw = std::accumulate(s_bw_array.begin(), s_bw_array.end(), 0) / s_bw_array.size();
            s_bw_max = *std::max_element(s_bw_array.begin(), s_bw_array.end());
            s_bw_max = (s_bw_max + s_avg_bw) * 1.2 / 2;
            s_bw_array.clear();
        }
    }

    // get the state whether adjust the encoder bitrate.
    int srtBitrateGetState(int inflight)
    {
        static constexpr int64_t INCR = 1316 * 8 * 3;
        static constexpr int64_t DECR = -1316 * 8 * 6;

        if (!s_rtt_ready || !s_bw_ready) {
            return STATE_KEEP;
        }

        const double bdp = s_bw_max * s_rtt_min / 1000;
        inflight = inflight * 8;
        const int64_t current_state = s_cwnd_gain * bdp - inflight;
        s_state_array.push_back(current_state);
        if (!(s_state_ready = s_state_array.size() >= STATE_LIST_MAX)) {
            return STATE_KEEP;
        }

        const int64_t final_state = std::accumulate(s_state_array.begin(), s_state_array.end(), 0l);
        llog("get congestion inflight:{}, _bw_max:{}, _bw_avg:{}, _rtt_min:{}, bdp:{}, final_state:{}, current_state:{}",
            inflight, s_bw_max, s_avg_bw, s_rtt_min, bdp, final_state, current_state);

        if (final_state > (INCR)) {
            return STATE_INCR;
        } else if (final_state < (DECR - 1)) {
            return STATE_DECR;
        }

        return STATE_KEEP;
    }

    void updateVencodeBitate(int64_t current_vencode_bitrate)
    {
        if(update_vencode_bitrate_task.run([this] {




            })){
            update_vencode_bitrate_task.reset();
        }
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
                s_congestion_state = srtBitrateGetState(inflight);

                llog("congestion ctrl({}) return {}, rtt:{}, inflight:{}, bw_bitrate:{}",
                    sock, srt_bstats_result, rtt, inflight, bw_bitrate);
            })) {
            srt_congestion_ctrl_task.reset();
        }
    }

    //    static void reset_vencode_bitrate(void) {
    //        const int64_t RESET_INTERVAL = 500;
    //        const double INCR_RATION = 1.08;
    //        const double DECR_RATION = 0.85;
    //        int64_t now_ts;
    //        char debug_buffer[1024];
    //        char oper_sz[80];

    //        if (!g_is_srt) {
    //            return;
    //        }

    //        now_ts = av_gettime()/1000;
    //        if (s_last_video_perf_ts == 0) {
    //            s_last_video_perf_ts = now_ts;
    //            return;
    //        }

    //        if ((now_ts - s_last_video_perf_ts) > RESET_INTERVAL) {
    //            if (s_congestion_state == STATE_INCR) {//not adjust bitrate for bandwith is enough
    //                s_current_bitrate = s_enc_ctx->bit_rate*INCR_RATION;

    //                if (s_current_bitrate < s_video_bitrate*1.3) {
    //                    snprintf(oper_sz, sizeof(oper_sz), "increase");
    //                    s_enc_ctx->bit_rate = s_current_bitrate;
    //                    s_enc_ctx->rc_max_rate = s_enc_ctx->rc_max_rate * INCR_RATION;

    //                    s_enc_ctx->rc_buffer_size = s_enc_ctx->rc_buffer_size * INCR_RATION;
    //                    s_enc_ctx->rc_initial_buffer_occupancy = s_enc_ctx->rc_initial_buffer_occupancy * INCR_RATION;
    //                } else {
    //                    snprintf(oper_sz, sizeof(oper_sz), "keep");
    //                }
    //                snprintf(debug_buffer, sizeof(debug_buffer),
    //                    "{\"timestamp\":\"%ld\", \"oper\":\"%s\", \"minrtt\":\"%d\", \"real_bitrate\":\"%ld\", \"conf_bitrate\":\"%ld\"}",
    //                    now_ts, oper_sz, s_rtt_min, s_current_bitrate, s_video_bitrate);
    //            } else if (s_congestion_state == STATE_DECR) {//adjust bitrate to estimate_bandwith cbr
    //                s_current_bitrate = s_enc_ctx->bit_rate*DECR_RATION;

    //                if (s_current_bitrate > s_video_bitrate*0.4) {
    //                    snprintf(oper_sz, sizeof(oper_sz), "decrease");
    //                    s_enc_ctx->bit_rate = s_current_bitrate;
    //                    s_enc_ctx->rc_max_rate = s_enc_ctx->rc_max_rate * DECR_RATION;

    //                    s_enc_ctx->rc_buffer_size = s_enc_ctx->rc_buffer_size * DECR_RATION;
    //                    s_enc_ctx->rc_initial_buffer_occupancy = s_enc_ctx->rc_initial_buffer_occupancy * DECR_RATION;
    //                } else {
    //                    snprintf(oper_sz, sizeof(oper_sz), "keep");
    //                }
    //                snprintf(debug_buffer, sizeof(debug_buffer),
    //                    "{\"timestamp\":\"%ld\", \"oper\":\"%s\", \"minrtt\":\"%d\", \"real_bitrate\":\"%ld\", \"conf_bitrate\":\"%ld\"}",
    //                    now_ts, oper_sz, s_rtt_min, s_current_bitrate, s_video_bitrate);
    //            } else {
    //                snprintf(oper_sz, sizeof(oper_sz), "keep");
    //                snprintf(debug_buffer, sizeof(debug_buffer),
    //                    "{\"timestamp\":\"%ld\", \"oper\":\"%s\", \"minrtt\":\"%d\", \"real_bitrate\":\"%ld\", \"conf_bitrate\":\"%ld\"}",
    //                    now_ts, oper_sz, s_rtt_min, s_current_bitrate, s_video_bitrate);
    //            }
    //            srt_log("%s\r\n", debug_buffer);
    //            //srt_log("%ld\r\n", s_current_bitrate);

    //            s_last_video_perf_ts = now_ts;
    //            return;
    //        }

    //        return;
    //    }

private:
    static constexpr auto VIDEO_UPDATE_INTERVAL = 500ms;
    static constexpr double INCR_RATION = 1.08;
    static constexpr double DECR_RATION = 0.85;

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

    std::vector<int> s_rtt_array;
    int s_rtt_min = RTT_INIT; // ms
    bool s_rtt_ready = false;

    std::vector<uint64_t> s_bw_array; // bits/s
    uint64_t s_bw_max = 0; // bits/s, max srt bandwidth
    uint64_t s_avg_bw = 0; // bits/s, avg srt bandwidth
    bool s_bw_ready = false;

    std::vector<int64_t> s_state_array;
    bool s_state_ready = false;

    double s_cwnd_gain = CWND_GAIN_DEF;
    int s_congestion_state = STATE_KEEP;

    int64_t s_video_bitrate = 0;
    int64_t s_current_bitrate = 0;
    int64_t s_last_video_perf_ts = 0;

    xlab::Task update_vencode_bitrate_task { 1, VIDEO_UPDATE_INTERVAL };
    xlab::Task srt_congestion_ctrl_task { 1, SRT_CHECK_INTERVAL };
};
