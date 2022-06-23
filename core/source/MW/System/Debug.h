#pragma once
#include <cstdio>
#include <cstdlib>

#ifdef NDEBUG
#  define MW_DEBUG 0
#else
#  define MW_DEBUG 1
#endif


#if MW_DEBUG

namespace mw {
inline namespace debug {
inline void DoAssert(const char* expr, const char* file, unsigned int line) {
    printf("Assertion failed: %s, file %s, line %u\n", expr, file, line);
    std::abort();
}
} // debug
} // mw

#define Assert(expression) (void)(                                                    \
            (!!(expression)) ||                                                       \
            (mw::debug::DoAssert(#expression, __FILE__, (unsigned int)(__LINE__)), 0) \
        )

#else
#define Assert(expression) ((void)0)
#endif
