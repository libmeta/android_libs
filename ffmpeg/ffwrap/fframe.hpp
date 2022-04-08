#pragma once

extern "C" {
#include "libavcodec/avcodec.h"
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

    ~FFrame()
    {
        av_frame_free(&handle_);
    }

    bool isNull()
    {
        return handle_ == nullptr || handle_->data[0] == nullptr || handle_->linesize[0] == 0;
    }

    void do(const DoType& func)
    {
        func(handle_);
    }

private:
    AVFrame* handle_;
};
