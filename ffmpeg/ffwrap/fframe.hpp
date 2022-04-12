#pragma once

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
}

#include <functional>

#include "xlog_common.hpp"

struct FFrame : public XLogLevelBase {
    using DoType = std::function<void(AVFrame*)>;

    explicit FFrame()
        : XLogLevelBase()
    {
        handle_ = av_frame_alloc();
    }

    explicit FFrame(const AVFrame* frame)
        : handle_(frame)
        , XLogLevelBase()
    {
    }

    ~FFrame()
    {
        av_frame_free(&handle_);
    }

    bool isNull() const
    {
        return handle_ == nullptr || handle_->data[0] == nullptr || handle_->linesize[0] == 0;
    }

    void do(const DoType& func)
    {
        func(handle_);
    }

    void log()
    {
        dlog("width:{}, height:{},key_frame:{},pict_type:{},format:{},pts:{},pkt_dts:{},nb_samples:{},sample_rate:{}",
            handle_->width,
            handle_->height,
            handle_->key_frame,
            handle_->pict_type,
            handle_->format,
            handle_->pts,
            handle_->pkt_dts,
            handle_->nb_samples,
            handle_->sample_rate);
    }

private:
    AVFrame* handle_;
};
