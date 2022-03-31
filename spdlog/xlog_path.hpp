#pragma once

#include <string>

#ifdef _WIN32|| WIN32
#include <codecvt>
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")

static inline std::string wstring2utf8string(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstr);
    //return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(wstr);//c++17
}

#endif

static inline std::string getDefaultXLogPath(){
#ifdef ANDROID
    return  "/sdcard/x/log/sdk/sdk.log";
#endif

#ifdef _WIN32
    WCHAR documents[MAX_PATH];
    HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents);
    if (result == S_OK) {
        return wstring2utf8string(documents)+"/x/log/sdk.log";
    }
#endif

    return "x/log/sdk.log";
}
