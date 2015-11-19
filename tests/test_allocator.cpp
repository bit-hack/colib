#include <stdint.h>
#include "colib.h"

#define test_assert(X) if (!(X)) {return -1;}

static
void thread_func(co_thread_t * self) {
}

int32_t test_allocator() {
    co_thread_t * thread = co_create (thread_func, 1024 * 512, nullptr);
    test_assert(thread);
    return 0;
}
