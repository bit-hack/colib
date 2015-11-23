#include <stdint.h>
#include "colib.h"

void thread_func(co_thread_t * co) {
    printf("2,");
    co_yield(co);
    printf("4,");
}

int32_t test_simple() {
    co_thread_t * host = co_init(nullptr);
    co_thread_t * thread = co_create (host, thread_func, 1024 * 512, nullptr);
    printf("1,");
    co_yield(host, thread);
    printf("3,");
    co_yield(host, thread);
    printf("5.\n");
    co_delete(thread);
    co_delete(host);
    return 0;
}
