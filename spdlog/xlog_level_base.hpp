#pragma once
#include <atomic>

#include "xlog.hpp"

class XLogLevelBase {
public:
    static inline auto XLogLevelDefault = XLog::ELevel::off;

public:
    virtual ~XLogLevelBase() {};

    virtual void setLevel(XLog::ELevel level)
    {
        this->level = level;
    }

    virtual XLog::ELevel getLevel() const
    {
        return this->level;
    }

private:
    std::atomic<XLog::ELevel> level = XLogLevelDefault;
};
