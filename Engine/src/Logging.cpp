#include "Logging.hpp"

#include <iostream>

namespace Neon
{
    LoggingLevel Logging::level = LoggingLevel::Info;
    std::string Logging::nameSpace = "Neon";

    void Logging::Init(std::string _nameSpace)
    {
        nameSpace = _nameSpace;
        level = LoggingLevel::Info;
    }

    void Logging::Info(std::string message)
    {
        if (level > LoggingLevel::Info)
            return; // If curent logging level is
                    // greater than Info then just skipp logg

        std::cout << "\033[32m" << "[" << nameSpace << "][INFO] - " << message << "\033[0m\n";
    }

    void Logging::Warning(std::string message)
    {
        if (level > LoggingLevel::Warning)
            return; // If curent logging level is
                    // greater than Info then just skipp logg

        std::cout << "\033[93m" << "[" << nameSpace << "][WARNING] - " << message << "\033[0m\n";
    }

    void Logging::Error(std::string message)
    {
        if (level > LoggingLevel::Error)
            return; // If curent logging level is
                    // greater than Error then just skipp logg

        std::cout << "\033[31m" << "[" << nameSpace << "][ERROR] - " << message << "\033[0m\n";
    }

}