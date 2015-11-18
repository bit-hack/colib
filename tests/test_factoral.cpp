#include <assert.h>
#include <stdint.h>
#include "colib.h"

extern int32_t test_factoral();

static const int STACK_SIZE = 1024 * 32;

static
int32_t co_factoral(co_thread_t * co, int32_t val) {
    assert (co);
    co_yield(co);
    return (val == 1 ? val : val * co_factoral (co, val - 1));
}

static
void thread_proc( co_thread_t * co ) {
    assert (co);
    int64_t result = co_factoral (co, 5);
    co_set_user(co, (void*)result);
    co_yield(co);
}

extern
int32_t test_factoral() {

    co_thread_t * thread = co_create(thread_proc, STACK_SIZE, nullptr);
    if (!thread)
        return -1;

    co_set_user(thread, (void*) ~0);

    for (int i=0; i<100; i++) {
        co_yield(thread);
        if (co_get_user(thread) != (void*)~0)
            break;
    }

    int64_t result = (int64_t)co_get_user(thread);

    co_delete(thread, nullptr);

    if (result != 120)
        return -2;

    return 0;
}
