# COLIB
## The Tiny cross-platform co-routine library

COLIB is a very lightweight implementation of co-routines for C++.  The library has been kept intentionally minimal and focus has been placed on support for a wide range of platforms.

Adding a new target is super easy.  Just read your ABI doc, implement one C++ function and two in assembly and you are done!

## Example:

```C++
#include <cstdio>
#include "colib.h"

void thread_func(co_thread_t * co) {
    printf("2,");
    co_yield(co);
    printf("4,");
}

int main() {
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
```

## Target support:

|Windows |Status     |
|--------|-----------|
| x86    | Stable    |
| x86_64 | Stable    |

|Linux    |Status    |  
|---------|----------|
| x86     | Stable   |  
| x64_64  | Stable   |  
| Mipsel  | Working  |  
| Armv7   | Working  |  
| Aarch64 | Todo     |  
| Mips64  | Todo     |  
| PowerPC | Todo     |  

|OS X    |Status       |
|--------|-------------|
| X86_64 | Working     |

|Compiler  |OS             |
|----------|---------------|
| GCC      | Linux         |
| CLANG    | Linux, OSX    |
| MSVC     | Windows       |
