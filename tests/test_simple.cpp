/*
 * This test ensures that the order of yielding is as expected.
 */
#include <stdint.h>
#include "test.h"
#include "coconfig.h"
#include "colib.h"

static int32_t value = 0;

static
void thread_func(co_thread_t * co) {
    assert(co);
    assert(co_get_user(co) == nullptr);

    value = 1;
    co_yield(co, nullptr);

    value = 2;
    co_yield(co);

    value = 3;
    co_yield(co);

    value = 4;
}

int32_t test_simple() {

    co_thread_t * host = co_init(nullptr);
    assert(host);

    co_thread_t * thread = co_create (host, thread_func, 1024 * 512, nullptr);
    assert(thread);
    assert(value == 0);

    co_yield(host, thread);
    assert(value == 1);
    assert(co_status(thread) == COLIB_STATUS_YIELDING);

    co_yield(host, thread);
    assert(value == 2);
    assert(co_status(thread) == COLIB_STATUS_YIELDING);

    co_yield(host, thread);
    assert(value == 3);
    assert(co_status(thread) == COLIB_STATUS_YIELDING);

    co_yield(host, thread);
    assert(value == 4);
    assert(co_status(thread) == COLIB_STATUS_ENDED);

    co_yield(host, thread);
    assert(co_status(thread) == COLIB_STATUS_ENDED);

    co_delete(thread);
    return 0;
}
