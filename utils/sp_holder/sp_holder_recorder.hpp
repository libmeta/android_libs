#pragma once


#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "json.hpp"
#include "xlog.hpp"

namespace xlab {

class SPHolderRecorder {
private:
    static inline std::mutex mutex;
    static inline std::unordered_map<std::string, std::unordered_set<void*>> holder_recorder;

public:
    static void Add(const char* caller, std::string& tag, void* ptr)
    {
        std::lock_guard<decltype(mutex)> lockGuard(mutex);
        holder_recorder[tag].insert(ptr);
        LogInfomation(caller);
    }

    static void Remove(const char* caller, void* ptr)
    {
        std::lock_guard<decltype(mutex)> lockGuard(mutex);
        bool founded = false;
        for (auto& [key, set] : holder_recorder) {
            if (!set.count(ptr)) {
                continue;
            }
            set.erase(ptr);
            if (set.empty()) {
                holder_recorder.erase(key);
            }
            founded = true;
            break;
        }

        if (!founded) {
            xlogi("SPHolderRecorder: Not Found Tag: {}", fmt::ptr(ptr));
        }

        LogInfomation(caller);
    }

private:
    static void LogInfomation(const char* caller)
    {
        std::ostringstream oss;
        oss << fmt::format("SPHolderRecorder: Caller => {}", caller) << std::endl;
        std::vector<std::string> contents;
        contents.reserve(holder_recorder.size());
        size_t length = 0;
        for (const auto& [key, set] : holder_recorder) {
            auto str = fmt::format("{:<25} => {:>2}", key, set.size());
            contents.emplace_back(str);
            length = str.length();
        }
        oss << fmt::format(" ╔═{0:═^{1}}═╗", "", length) << std::endl;
        for (const auto& str : contents) {
            oss << fmt::format(" ║ {0} ║ ", str) << std::endl;
        }
        oss << fmt::format(" ╚═{0:═^{1}}═╝", "", length) << std::endl;
        clogi(oss.str());
    }
};

}

