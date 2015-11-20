#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER)
  #define WIN32_LEAN_AND_MEAN
  #define VC_EXTRALEAN
  #include <Windows.h>
#else
  #include <signal.h>
#endif

typedef int32_t (*test_func_t)();

extern int32_t test_simple   ();
extern int32_t test_factoral ();
extern int32_t test_multiple ();
extern int32_t test_allocator();
extern int32_t test_fpregs   ();
extern int32_t test_unchained();
extern int32_t test_yieldmain();
extern int32_t test_nesting  ();
extern int32_t test_params   ();

struct test_t {
    test_func_t func_;
    const char * name_;
};

test_t tests[] = {
    { test_simple,    "test_simple"    },
    { test_factoral,  "test_factoral"  },
    { test_multiple,  "test_multiple"  },
    { test_allocator, "test_allocator" },
    { test_fpregs,    "test_fpregs"    },
    { test_unchained, "test_unchained" },
    { test_yieldmain, "test_yieldmain" },
    { test_nesting,   "test_nesting"   },
    { test_params,    "test_params"    },
    { nullptr, nullptr }
};

const char * test_name = "";

#if defined(_MSC_VER)
LONG CALLBACK segv_handler(PEXCEPTION_POINTERS info) {
    printf("! crash '%s'\n", test_name);
#if defined(_M_X64)
    printf("  pc  @ 0x%016llx\n", info->ContextRecord->Rip);
    printf("  sp  @ 0x%016llx\n", info->ContextRecord->Rsp);
#else
    printf("  pc  @ 0x%08lx\n", info->ContextRecord->Eip);
    printf("  sp  @ 0x%08lx\n", info->ContextRecord->Esp);
#endif
    getchar();
    exit(1);
}

void setup() {
    AddVectoredExceptionHandler(1, segv_handler);
}
#else
void segv_handler(int signal) {
    printf("! crash '%s'\n", test_name);
    exit(1);
}

void setup() {
    signal(SIGSEGV, segv_handler);
    signal(SIGABRT, segv_handler);
    signal(SIGINT,  segv_handler);
    signal(SIGILL,  segv_handler);
}
#endif

int main() {

    setup();
    int32_t fails = 0;

    for (int32_t i = 0; tests[i].name_; i++) {

        test_t & test = tests[i];
        test_name = test.name_;

        // run this specific test
        int32_t res = test.func_();

        if (res >= 0) {
            printf(". pass  '%s'\n", test.name_);
        }
        else {
            printf("! fail  '%s' (%d)\n", test.name_, res);
            ++fails;
        }
    }

    return fails;
}
