#pragma once

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/timestamp.h"
}

#include <functional>

#include "xlog_common.hpp"

struct FFPacket : public XLogLevelBase {
    using DoType = std::function<void(AVPacket*)>;

    explicit FFPacket()
        : XLogLevelBase()
    {
        handle_ = av_packet_alloc();
    }

    explicit FFPacket(const AVPacket* pkt)
        : handle_(pkt)
        , XLogLevelBase()
    {
    }

    ~FFPacket()
    {
        av_packet_free(&handle_);
    }

    bool isNull()
    {
        return handle_ == nullptr || handle_->data == nullptr || handle_->side_data_elems == 0;
    }

    void do(const DoType& func)
    {
        func(handle_)
    }

    void log(const AVFormatContext* fmt_ctx)
    {
        const auto pkt = handle_;
        AVRational* time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
        dlog("pts:{} pts_time:{} dts:{} dts_time:{} duration:{} duration_time:{} stream_index:{}",
            av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
            av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
            av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
            pkt->stream_index);
    }

private:
    AVPacket* handle_;
};
