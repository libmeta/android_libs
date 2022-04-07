#pragma once

#include <mutex>
#include <spdlog/spdlog.h>

#include "spdlog/async.h"
#include "spdlog/fmt/ostr.h" // must be included
#include "spdlog/sinks/android_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "xlog_path.hpp"

class XLog final {
    static inline std::mutex LoggerLock;
    static inline const char* ConsoleLoggerName = "console";
    static inline const char* TextLoggerName = "text";
    static inline std::string TextLoggerPath = getDefaultXLogPath();

public:
    using ELevel = spdlog::level::level_enum;
    using logger = spdlog::logger;

    ~XLog()
    {
        // spdlog::shutdown();
    }

    static void setTextPath(std::string path)
    {
        TextLoggerPath = path;
    }

    static XLog& getInstance()
    {
        static XLog inst;
        return inst;
    }

    static std::shared_ptr<spdlog::logger> console()
    {
        static auto console = XLog::get();
        return console;
    }

    static std::shared_ptr<spdlog::logger> text()
    {
        static auto text = XLog::get(TextLoggerName, TextLoggerPath);
        return text;
    }

    template <typename... Args>
    void log(spdlog::level::level_enum lvl, fmt::format_string<Args...> fmt, Args&&... args)
    {
        console()->log(lvl, fmt, std::forward<Args>(args)...);
        text()->log(lvl, fmt, std::forward<Args>(args)...);
    }

private:
    XLog() = default;

    XLog(const XLog&) = delete;

    XLog& operator=(const XLog&) = delete;

    static std::shared_ptr<spdlog::logger> get()
    {
        std::lock_guard<std::mutex> Locker(LoggerLock);
        std::shared_ptr<spdlog::logger> log = spdlog::get(ConsoleLoggerName);
        if (log == nullptr) {
            log = CreateConsoleLog(ConsoleLoggerName);
        }
        return log;
    }

    static std::shared_ptr<spdlog::logger> get(std::string logger_name, std::string path)
    {
        std::lock_guard<std::mutex> Locker(LoggerLock);
        std::shared_ptr<spdlog::logger> log = spdlog::get(logger_name);
        if (log == nullptr) {
            log = CreateTextLog(logger_name, path);
        }
        return log;
    }

    static std::shared_ptr<spdlog::logger> CreateConsoleLog(std::string logger_name)
    {
#ifdef ANDROID
        auto console_logger = spdlog::android_logger_mt(logger_name, "sdk");
        console_logger->set_pattern("%v");
        console_logger->set_level(spdlog::level::info);
        console_logger->flush_on(spdlog::level::info);
        return console_logger;
#else
        auto console_logger = spdlog::stdout_logger_mt(logger_name);
        console_logger->set_pattern("[%L] [%H:%M:%S.%e] [thread %t] %v");
        console_logger->set_level(spdlog::level::info);
        console_logger->flush_on(spdlog::level::info);
        return console_logger;
#endif
    }

    /// logger_name 不能重名
    static std::shared_ptr<spdlog::logger> CreateTextLog(std::string logger_name,
        std::string path,
        spdlog::level::level_enum lvl = spdlog::level::info)
    {
        spdlog::init_thread_pool((8L * 1024), 1);
        auto async_file = spdlog::daily_logger_mt<spdlog::async_factory>(logger_name, path, 2, 30);
        async_file->set_pattern("[%L] [%H:%M:%S.%e] [thread %t] %v");
        async_file->set_level(lvl);
        async_file->flush_on(lvl);
        return async_file;
    }
};

///打印指针用fmt::ptr()转换
//#ifndef __FILE_NAME__
#ifdef _WIN32 || WIN32
#include <stdlib.h>
#include <string.h>
static inline std::string getFileName(const char* path)
{
    std::string fname(64, 0);
    std::string ext(16, 0);
    _splitpath(path, NULL, NULL, fname.data(), ext.data());
    fname.resize(strlen(fname.c_str()));
    return fname + ext;
}

#define __FILE_NAME__ getFileName(__FILE__).c_str()
#else
#include <libgen.h>
#define __FILE_NAME__ (basename(__FILE__))
#endif
//#endif

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#ifndef xlog
#define xlog XLog::getInstance()
#endif

#ifndef XLevel
#define XLevel XLog::ELevel
#endif

#ifndef xlogt
#define xlogt(fmt, ...) XLog::getInstance().log(spdlog::level::trace, "[{}:{}] [{}] : " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

#ifndef xlogd
#define xlogd(fmt, ...) XLog::getInstance().log(spdlog::level::debug, "[{}:{}] [{}] : " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

#ifndef xlogi
#define xlogi(fmt, ...) XLog::getInstance().log(spdlog::level::info, "[{}:{}] [{}] : " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

#ifndef xlogw
#define xlogw(fmt, ...) XLog::getInstance().log(spdlog::level::warn, "[{}:{}] [{}] : " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

#ifndef xloge
#define xloge(fmt, ...) XLog::getInstance().log(spdlog::level::err, "[{}:{}] [{}] : " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

#ifndef xlogc
#define xlogc(fmt, ...) XLog::getInstance().log(spdlog::level::critical, "[{}:{}] [{}] : " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

#ifndef clogt
#define clogt(fmt, ...) XLog::getInstance().log(spdlog::level::trace, fmt, ##__VA_ARGS__)
#endif

#ifndef clogd
#define clogd(fmt, ...) XLog::getInstance().log(spdlog::level::debug, fmt, ##__VA_ARGS__)
#endif

#ifndef clogi
#define clogi(fmt, ...) XLog::getInstance().log(spdlog::level::info, fmt, ##__VA_ARGS__)
#endif

#ifndef clogw
#define clogw(fmt, ...) XLog::getInstance().log(spdlog::level::warn, fmt, ##__VA_ARGS__)
#endif

#ifndef cloge
#define cloge(fmt, ...) XLog::getInstance().log(spdlog::level::err, fmt, ##__VA_ARGS__)
#endif

#ifndef clogc
#define clogc(fmt, ...) XLog::getInstance().log(spdlog::level::critical, fmt, ##__VA_ARGS__)
#endif

#ifndef dlog
#define dlog(level, fmt, ...) XLog::getInstance().log(level, fmt, ##__VA_ARGS__)
#endif

#ifndef llog
#define llog(fmt, ...) dlog(this->getLevel(), fmt, ##__VA_ARGS__)
#endif
