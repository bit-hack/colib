/*
 * This test yields between threads in a quasi random order to make sure
 * that there are no yield order dependencies.
 */
#include <stdint.h>
#include "test.h"
#include "colib.h"

static const uint32_t num_threads = 64;

struct args_t {
    co_thread_t * thread_[num_threads];
    co_thread_t * host_;
    uint32_t val_;
};

static
void thread_func(co_thread_t * self) {
    args_t & args = * (args_t*) co_get_user(self);
    for (uint32_t to=999999;to;--to) {
        if (--args.val_) {
            uint32_t next = rand64() % num_threads;
            co_yield(self, args.thread_[next]);
        }
        else {
            co_yield(self, args.host_);
        }
    }
}

int32_t test_unchained() {

    args_t args;
    args.host_ = co_init(nullptr);
    args.val_ = 100000;
    
    for (uint32_t i = 0; i < num_threads; ++i) {
        args.thread_[i] = co_create (args.host_, thread_func, 1024, nullptr);
        assert(args.thread_[i]);
        co_set_user(args.thread_[i], &args);
    }
    co_yield(args.host_, args.thread_[0]);
    assert(args.val_ == 0);
    return 0;
}
