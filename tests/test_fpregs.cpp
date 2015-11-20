/*
 * This test uses floating point registers to check they don't get
 * clobbered between yields.
 */
#include <stdint.h>
#include <math.h>
#include "test.h"
#include "colib.h"

// this coroutine function computes an iterative square route
static
void thread_func(co_thread_t * self) {

    float * ans = (float*)co_get_user(self);
    float x = * ans;

    int32_t exp = 0;
    x = frexp(x, &exp);
    if (exp & 1) {
        exp--;
        x *= 2;
    }
    float y = (1+x)/2;
    float z = 0;
    for (uint32_t to=1000; to>0; --to)
    {
        z = y;
        y = (y + x/y) / 2;
        *ans = ldexpf(y, exp/2);

        co_yield(self, nullptr);
    }

    //todo: Fail, we hit the iteration timeout
}

int32_t test_fpregs() {

    const uint32_t num_threads = 3;
    const uint32_t num_itters = 128;

    float scratch[num_threads] = {
        823345.234f,
        643.124f,
        4.823f
    };

    float result[num_threads];
    co_thread_t * thread[num_threads];
    co_thread_t * host = co_init(nullptr);

    // fill the result buffer
    for (uint32_t i=0; i<num_threads; ++i )
        result[i] = sqrtf(scratch[i]);

    // create all of the threads
    for (uint32_t i=0; i<num_threads; ++i) {
        thread[i] = co_create (host, thread_func, 1024 * 512, nullptr);
        assert(thread[i]);
        co_set_user(thread[i], scratch+i);
    }

    // iterate and yield in a random pattern
    for (uint32_t i=0; i<num_itters; ++i) {
        uint32_t j = rand64() % num_threads;
        co_yield(host, thread[j]);
    }

    // check our results are not too far off
    for (uint32_t i=0; i<num_threads; ++i) {
        float diff = fabsf(scratch[i]-result[i]);
        if (diff > 0.01f)
            return -1;
    }

    // success
    return 0;
}
