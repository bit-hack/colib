#pragma once

#if defined(_MSC_VER)
  #include <intrin.h>
  #define assert(X) {if (!(X)) __debugbreak();}
#else
  #include <signal.h>
  #define assert(X) {if (!(X)) raise(SIGINT);}
#endif

namespace {
// simple 64bit pseudo random number generator
uint64_t rand64(void) {
    static uint64_t x = 0x65d1fd6d32f552edull;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    return x * 0x2545f4914f6cdd1dull;
}
} // namespace {}
