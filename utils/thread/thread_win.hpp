#pragma once

#include <string>
#include <thread>

#ifdef _WIN32 || WIN32
#include <windows.h>
static const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

static inline void SetThreadName(const std::string& name, std::thread::native_handle_type tid = GetCurrentThreadId())
{
    if (name.empty()) {
        return;
    }

    char pname[32] = { 0 };
    const auto len = name.size();
    memcpy(pname, name.c_str(), len > 16 ? 16 : len);

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = pname.c_str();
    info.dwThreadID = tid;
    info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable : 6320 6322)
    __try {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
#pragma warning(pop)
}

#endif
