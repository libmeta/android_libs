#pragma once

#include <string>
#include <thread>

#ifndef _WIN32&& WIN32

static inline void SetThreadName(const std::string& name, std::thread::native_handle_type tid = pthread_self())
{
    if (name.empty()) {
        return;
    }

    char pname[32] = { 0 };
    const auto len = name.size();
    memcpy(pname, name.c_str(), len > 16 ? 16 : len);
    pthread_setname_np(tid, pname);
}

#endif
