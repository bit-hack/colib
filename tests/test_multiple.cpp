#include <assert.h>
#include <stdint.h>
#include "colib.h"

extern int32_t test_multiple();

static const int THREAD_SIZE = 1024 * 32;

struct stack_t {

    static const int MAX = 64;
    uint64_t data_[MAX];
    uint64_t head_;

    stack_t()
        : head_(0)
    {
    }

    void push(uint64_t v) {
        assert(head_<MAX);
        data_[head_++] = v;
    }

    uint64_t pop() {
        assert(head_>0);
        return data_[--head_];
    }

    uint64_t size() const {
        return head_;
    }

    bool full() const {
        return head_ >= MAX;
    }

    bool empty() const {
        return head_ == 0;
    }
};

struct co_thread_ex_t {

    co_thread_ex_t(co_func_t func, uint32_t size, co_allocator_t *alloc)
        : alloc_(alloc)
    {
        t_ = co_create(func, size, alloc);
    }

    ~co_thread_ex_t() {
        if (t_)
            co_delete(t_, alloc_);
    }

    bool alive() const {
        if (!t_)
            return false;
        return co_status(t_) == COLIB_STATUS_YIELDING;
    }

    void set_user(void*dat) {
        if (t_)
            co_set_user(t_, dat);
    }

    void * get_user() const {
        if (t_)
            return co_get_user(t_);
        return nullptr;
    }

    void yield() {
        if (t_)
            co_yield(t_);
    }

    co_thread_t * t_;
    co_allocator_t * alloc_;
};

struct params_t {
    stack_t stack_;
    uint64_t val_;
};

static
uint64_t rand64(void) {
    static uint64_t x = 1;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    return x * UINT64_C(2685821657736338717);
}

static
void thread_proc_a(co_thread_t * co) {
    assert (co);
    params_t * params = (params_t*)co_get_user(co);
    while (true) {
        while (params->stack_.full()) {
            co_yield(co);
        }
        params->stack_.push(rand64());
    }
}

static
void thread_proc_b(co_thread_t * co) {
    assert (co);
    params_t * params = (params_t*)co_get_user(co);
    while (true) {
        while (params->stack_.empty()) {
            co_yield(co);
        }
        params->val_ += params->stack_.pop();
    }
}

extern
int32_t test_multiple() {

    params_t params;
    params.val_ = 0;

    co_thread_ex_t t1(thread_proc_a, THREAD_SIZE, nullptr);
    co_thread_ex_t t2(thread_proc_b, THREAD_SIZE, nullptr);
    co_thread_ex_t t3(thread_proc_b, THREAD_SIZE, nullptr);

    if (! (t1.alive() && t2.alive() && t3.alive()))
        return -1;

    t1.set_user(&params);
    t2.set_user(&params);
    t3.set_user(&params);

    for (int i=0; i<100; i++) {
        t1.yield();
        t2.yield();
        t3.yield();
    }

    if (params.val_ != 12886961605791636653ull)
        return -2;

    return 0;
}
