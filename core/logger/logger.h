#ifndef CORE_LOGGER_LOGGER_H
#define CORE_LOGGER_LOGGER_H

#include <cstdio>
#include <utility>

#define akLogger Logger::GetInstance()

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

        template <typename... Args>
        void LogTrace(const char *string, Args &&...args) { Log(Levels::Trace, string, std::forward(args...)); }

        template <typename... Args>
        void LogDebug(const char *string, Args &&...args) { Log(Levels::Debug, string, std::forward<Args>(args)...); }

        template <typename... Args>
        void LogInfo(const char *string, Args &&...args) { Log(Levels::Info, string, std::forward(args...)); }

        template <typename... Args>
        void LogWarning(const char *string, Args &&...args) { Log(Levels::Warning, string, std::forward(args...)); }

        template <typename... Args>
        void LogError(const char *string, Args &&...args) { Log(Levels::Error, string, std::forward(args...)); }

        template <typename... Args>
        void LogFatal(const char *string, Args &&...args) { Log(Levels::Fatal, string, std::forward(args...)); }

        inline void SetLevel(Levels value) { m_Level = value; }
        inline Levels GetLevel() { return m_Level; }

        static Logger &GetInstance();
        

    private:
        template <typename... Args>
        void Log(Levels level, const char *log, Args &&...args)
        {
            if (level <= m_Level)
            {
                printf(log, std::forward<Args>(args)...);
            }
        }

        Levels m_Level = Levels::Debug;
    };

} // namespace ak::core

#endif // #ifndef CORE_LOGGER_LOGGER_H
