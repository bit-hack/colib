/*
 * This test ensures that the order of yielding is as expected.
 */
#include <stdint.h>
#include "test.h"
#include "colib.h"

static void * param1 = (void*) "param1";
static void * param2 = (void*) "param2";
static void * param3 = (void*) "param3";

static
void thread_func(co_thread_t * co) {
    assert(co);
    assert(co_get_user(co) == param1);
    co_yield(co);
    assert(co_get_user(co) == param2);
    co_set_user(co, param3);
}

int32_t test_params() {

    co_thread_t * host = co_init(nullptr);
    assert(host);

    co_thread_t * thread = co_create (host, thread_func, 1024, nullptr, param1);
    assert(thread);
    assert(co_get_user(thread) == param1);

    co_yield(host, thread);
    co_set_user(thread, param2);

    co_yield(host, thread);
    assert(co_get_user(thread) == param3);

    co_delete(thread);
    co_delete(host);
    return 0;
}
