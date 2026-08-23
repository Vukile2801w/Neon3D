#ifndef NEON_ASSERT
#define NEON_ASSERT

#include <cstdlib>

#include "Logging.hpp"

#ifdef NEON_DEBUG

#define NEON_ASSERT(condition, message)    \
    do                                     \
    {                                      \
        if (!(condition))                  \
        {                                  \
            Logging::Error(                \
                std::string("[ASSERT] ") + \
                __FILE__ +                 \
                ":" +                      \
                std::to_string(__LINE__) + \
                ": " +                     \
                message);                  \
            std::abort();                  \
        }                                  \
    } while (false)

#else

#define NEON_ASSERT(condition, message) \
    do                                  \
    {                                   \
        (void)sizeof(condition);        \
    } while (false)

#endif

#endif
