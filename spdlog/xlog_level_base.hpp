#pragma once

#include <atomic>

#include "xlog.hpp"

class XLogLevelBase {
public:
    XLogLevelBase(XLog::ELevel console_level = XLog::ELevel::trace, XLog::ELevel text_level = XLog::ELevel::trace)
        : consoleLevel(console_level)
        , textLevel(text_level)
    {
    }

    virtual ~XLogLevelBase() { }

    virtual void setLevel(XLog::ELevel level)
    {
        setConsoleLevel(level);
        setTextLevel(level);
    }

    virtual void setConsoleLevel(XLog::ELevel level)
    {
        this->consoleLevel = level;
    }

    virtual void setTextLevel(XLog::ELevel level)
    {
        this->textLevel = level;
    }

    virtual XLog::ELevel getConsoleLevel() const
    {
        return this->consoleLevel;
    }

    virtual XLog::ELevel getTextLevel() const
    {
        return this->textLevel;
    }

private:
    std::atomic<XLog::ELevel> consoleLevel;
    std::atomic<XLog::ELevel> textLevel;
};
