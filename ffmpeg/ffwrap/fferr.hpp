#pragma once

#include <string>

extern "C" {
#include <libavutil/error.h>
}

namespace FFErr {
static inline const std::string& toString(int ret)
{
    auto err = av_err2str(ret);
    if (err == nullptr) {
        return "";
    }

    return std::string(err) + "(" + std::to_string(ret) + ")";
}

}
