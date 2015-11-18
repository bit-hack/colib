#include <stdint.h>
#include <stdio.h>

extern int32_t test_simple();
extern int32_t test_factoral();
extern int32_t test_multiple();

typedef int32_t (*test_func_t)();

struct test_t {
    test_func_t func_;
    const char * name_;
};

test_t tests[] = {
    { test_simple,   "test_simple" },
    { test_factoral, "test_factoral" },
    { test_multiple, "test_multiple" },
    { nullptr, nullptr }
};

int main() {

    int fails = 0;

    for (int i = 0; tests[i].name_; i++) {

        test_t & test = tests[i];
        // run the test
        int32_t res = test.func_();

        if (res >= 0) {
            printf(". pass '%s'\n", test.name_);
        }
        else {
            printf("! fail '%s' (%d)\n", test.name_, res);
            ++fails;
        }
    }

    return fails;
}
