#include <stdint.h>
#include "colib.h"

#define test_assert(X) if (!(X)) {return -1;}

// todo: test the unchained mode using random number generator to yield
//       between coroutines randomly over thousands of iterations

static
void thread_func(co_thread_t * self) {
}

int32_t test_unchained() {
    co_thread_t * thread = co_create (thread_func, 1024 * 512, nullptr);
    test_assert(thread);
    return 0;
}
