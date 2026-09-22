#ifndef NEON_LOGGING
#define NEON_LOGGING

#include <string>

namespace Neon
{
    enum class LoggingLevel
    {
        Info,
        Warning,
        Error,
        None
    };

    class Logging
    {
    public:
        static void Init(std::string _nameSpace);

        static void Info(std::string message);
        static void Warning(std::string message);
        static void Error(std::string message);

        static LoggingLevel level;
        static std::string nameSpace;
    };
}

#endif