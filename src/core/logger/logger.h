#ifndef CORE_LOGGER_LOGGER_H
#define CORE_LOGGER_LOGGER_H

#include <cstdio>
#include <utility>

#define akLogger ak::core::Logger::GetInstance()

#define akLogEx(level, ...)                                                            \
    if (ak::core::Logger::GetInstance().GetLevel() <= ak::core::Logger::Levels::level) \
    {                                                                                  \
        printf("[LOG " #level"]: " __VA_ARGS__);                                                           \
    }

#define akLogTrace(...) akLogEx(Trace, __VA_ARGS__)
#define akLogDebug(...) akLogEx(Debug, __VA_ARGS__)
#define akLogInfo(...) akLogEx(Info, __VA_ARGS__)
#define akLogWarning(...) akLogEx(Warning, __VA_ARGS__)
#define akLogError(...) akLogEx(Error, __VA_ARGS__)
#define akLogFatal(...) akLogEx(Fatal, __VA_ARGS__)

namespace ak::core
{
    class Logger
    {
    public:
        enum class Levels
        {
            Trace = 0,
            Debug,
            Info,
            Warning,
            Error,
            Fatal,
            Off
        };

        inline void SetLevel(Levels value) { m_Level = value; }
        inline Levels GetLevel() { return m_Level; }

        static Logger &GetInstance();

    private:
        Levels m_Level = Levels::Trace;
    };

} // namespace ak::core

#endif
