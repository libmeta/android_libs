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

    bool init(){

    }


    void deInit(){

    }

private:
    AVDictionary* options = nullptr;
};
