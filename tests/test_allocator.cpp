#if 0
#include <stdint.h>
#include "colib.h"

#define test_assert(X) if (!(X)) {return -1;}

extern int32_t test_simple();

static int32_t value = 0;

static
void thread_func(co_thread_t * co) {
    value = 1;
    co_yield(co);
    value = 2;
    co_yield(co);
    value = 13;
    co_yield(co);
    value = 29;
}

int32_t test_simple() {
    co_thread_t * thread = co_create (thread_func, 1024 * 512, nullptr);
    test_assert(thread);
    test_assert(value == 0);
    co_yield(thread);
    test_assert(value == 1);
    co_yield(thread);
    test_assert(value == 2);
    co_yield(thread);
    test_assert(value == 13);
    co_yield(thread);
    test_assert(value == 29);
    co_yield(thread);
    co_delete(thread, nullptr);
    return 0;
}
#endif