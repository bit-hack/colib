#include <cstdio>
#include "colib.h"

void thread_func(co_thread_t * co) {
    printf("2,");
    co_yield(co);
    printf("4,");
}

int main() {
    co_thread_t * host = co_init(0);
    co_thread_t * thread = co_create (host, thread_func, 1024 * 512, 0);
    printf("1,");
    co_yield(host, thread);
    printf("3,");
    co_yield(host, thread);
    printf("5.\n");
    co_delete(thread);
    co_delete(host);
    return 0;
}
