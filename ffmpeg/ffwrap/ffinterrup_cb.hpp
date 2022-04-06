#pragma once

extern "C" {
#include "libavformat/avio.h"
}

#include <string>

class FFInterruptCB final {
public:
    static int InterruptCallback(void* arg)
    {
        if (arg == nullptr) {
            return 1;
        }
        auto thiz = static_cast<FFInterruptCB*>(arg);
        if (thiz->isExit) {
            return 1;
        }

        return 0;
    }

    explicit FFInterruptCB()
        : isExit(false)
    {
    }

    ~FFInterruptCB()
    {
        isExit = true;
    }

    AVIOInterruptCB GetAVIOInterruptCB()
    {
        return { &FFInterruptCB::InterruptCallback, this };
    }

    void Exit()
    {
        isExit = true;
    }

    const bool IsExit() const
    {
        return isExit;
    }

private:
    std::atomic<bool> isExit = false;
};
