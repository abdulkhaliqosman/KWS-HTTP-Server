#include "logger.h"

namespace ak::core
{
    Logger &Logger::GetInstance()
    {
        static Logger logger;
        return logger;
    }
}
