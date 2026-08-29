#ifndef NEON_REF
#define NEON_REF

#include <memory>

namespace Neon
{
    template <typename T>
    using Ref = std::shared_ptr<T>;
}
#endif