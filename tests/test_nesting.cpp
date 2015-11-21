/*
 * Check that one coroutine can create another coroutine.
 */
#include <stdint.h>
#include "test.h"
#include "coconfig.h"
#include "colib.h"

#define test_assert(X) if (!(X)) {return -1;}

static const uint32_t num_threads = 128;

struct args_t {
    uint32_t head_;
    co_thread_t * thread_[num_threads];
};

static
void thread(co_thread_t * self) {
    assert(co_get_user(self));
    args_t & args = * (args_t*) co_get_user(self);

    if (args.head_ >= num_threads) {
        co_yield_to_main(self);
    }
    else {
        co_thread_t * & next = args.thread_[args.head_++];
        next = co_create(self, thread, 1024 * 512, nullptr, &args);
        co_yield(self, next);
    }

    assert(!"Should not get here");
}

int32_t test_nesting() {

    args_t args = {0};

    co_thread_t * host = co_init(nullptr);

    co_thread_t * t = co_create (host, thread, 1024 * 512, nullptr, &args);

    co_yield(host, t);
    test_assert(args.head_ == num_threads);

    for (uint32_t i = 0; i < num_threads; ++i)
        co_delete(args.thread_[i]);
    co_delete(host);

    return 0;
}
