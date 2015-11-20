/*
 * This test checks that the custom memory allocators are called and
 * used correctly.
 */

#include <stdint.h>
#include <stdlib.h>
#include "test.h"
#include "colib.h"

static void * param = (void*) "test";

static uint32_t num_allocs = 0;

static
void * al_alloc(uint32_t size, void * user) {
    assert(size > 0);
    assert(user == param);
    ++num_allocs;
    return malloc(size);

}

static
void al_free(void * mem, void * user) {
    assert(mem != nullptr);
    assert(user == param);
    --num_allocs;
    free(mem);
}

static
void thread_func(co_thread_t * self) {
}

int32_t test_allocator() {

    assert(num_allocs == 0);
    uint32_t old_allocs = num_allocs;

    co_allocator_t alloc = {al_alloc, al_free, param};

    co_thread_t * host = co_init(&alloc);
    assert(host);
    assert(num_allocs > old_allocs);
    old_allocs = num_allocs;

    co_thread_t * thread = co_create (host, thread_func, 1024 * 512, &alloc);
    assert(thread);
    assert(num_allocs > old_allocs);
    old_allocs = num_allocs;

    co_delete(thread);
    assert(num_allocs < old_allocs);
    old_allocs = num_allocs;

    co_delete(host);
    assert(num_allocs < old_allocs);
    old_allocs = num_allocs;

    return 0;
}
