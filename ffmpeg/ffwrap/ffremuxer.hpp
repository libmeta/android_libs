#pragma once

#include <memory>
#include <string>
#include <vector>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/channel_layout.h"
#include "libavutil/opt.h"
}

#include "fferr.hpp"
#include "ffinterrup_cb.hpp"
#include "xlog_common.hpp"

class FFRemuxer : public XLogLevelBase {
public:
    using HandleType = int;

private:
    explicit FFRemuxer()
        : XLogLevelBase()
    {
    }

public:
    static std::shared_ptr<FFRemuxer> Make(const std::string& inUrl, const std::string& outUrl)
    {
        auto muxer = std::shared_ptr<FFRemuxer>(new FFRemuxer());
        if (muxer->init(inUrl, outUrl)) {
            return muxer;
        }

        lllog(muxer->getConsoleLevel(), muxer->getTextLevel(), "{}", FFErr::toString(muxer->getCode()));
        return nullptr;
    }

    ~FFRemuxer()
    {
        deInit();
    }

    HandleType getHandle() const
    {
        int64_t srt_socket = 0;
        av_opt_get_int(options, "handle", 0, &srt_socket);
        return (HandleType)(srt_socket);
    }

    bool write(AVPacket* packet)
    {
        av_packet_rescale_ts(packet, AV_TIME_BASE_Q, outFmtCtx->streams[packet->stream_index]->time_base);
        const int write_result = av_write_frame(outFmtCtx, packet);
        code = write_result;
        return write_result < 0;
    }

    int getCode() const
    {
        return code;
    }

    bool isExit() const
    {
        return InterruptCB.IsExit();
    }

    void requestExit()
    {
        InterruptCB.Exit();
    }

private:
    bool init(const std::string& inUrl, const std::string& outUrl)
    {
        return true;
    }

    void deInit()
    {
    }

private:
    std::atomic<int> code = 0;
    FFInterruptCB InterruptCB;
    AVDictionary* options = nullptr;
    AVFormatContext* inFmtCtx = nullptr;
    AVFormatContext* outFmtCtx = nullptr;
};
