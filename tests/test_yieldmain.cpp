/*
 * This test checks that the custom memory allocators are called and
 * used correctly.
 */
#include <stdint.h>
#include "test.h"
#include "coconfig.h"
#include "colib.h"

struct args_t {
    co_thread_t * a_;
    co_thread_t * b_;
    co_thread_t * host_;
};

static
void thread_a(co_thread_t * self) {
    assert(co_get_user(self));
    args_t & args = * (args_t*) co_get_user(self);
    co_yield(self, args.b_);
    assert(!"Should not get here");
}

static
void thread_b(co_thread_t * self) {
    assert(co_get_user(self));
    co_yield_to_main(self);
    assert(!"Should not get here");
}

int32_t test_yieldmain() {

    args_t args;
    args.host_ = co_init(nullptr);

    args.a_ = co_create (args.host_, thread_a, 1024, nullptr, &args);
    args.b_ = co_create (args.host_, thread_b, 1024, nullptr, &args);

    co_yield(args.host_, args.a_);

    co_delete(args.a_);
    co_delete(args.b_);
    co_delete(args.host_);

    return 0;
}
