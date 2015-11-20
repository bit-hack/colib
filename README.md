# COLIB
## The Tiny cross-platform co-routine library

COLIB is a very lightweight implementation of co-routines for C++.  The library has been kept intentionally minimal and focus has been placed on support for a wide range of platforms.

Adding a new target is super easy.  Just read your ABI doc, implement one C++ function and two in assembly and you are done!

## Target support:

Platform support

|Windows |Status     |
|--------|-----------|
| x86    | Stable    |
| x86_64 | Stable    |

|Linux    |Status       |  
|---------|-------------|
| AMD64   | Stable      |  
| i386    | Stable      |  
| Mipsel  |             |  
| Armv7   |             |  
| Aarch64 |             |  
| Mips64  |             |  
| PowerPC |             |  

|OS X    |Status       |
|--------|-------------|
| X86_64 | In Progress |

Compiler support:

|Linux     |Windows        |
|----------|---------------|
| GCC      | Visual Studio |
| CLANG    |               |


## Todo:

- Better documentation
- New targets
- More tests
- More examples
- Add automated testing using cross compiler and QEMU
- Investigate other assemblers (NASM)
- Target defaults to system in Cmake


## The COLIB API

The Colib api follows the philosophy that less is more.  Just enought functionality is present to implement co-routines and no more.

---
#### co_create()
This function creates a new co-routine instance, encapsulated in the returned co_thread_t object. 

```
Arguments:
  func  - A pointer to the thread function for this co-routine to execute.
  size  - The size of the co-routine stack to create. 
  alloc - A custom allocator to be used for all allocations (optional).

Returns:
  The new co-routine encapsulated in a co_thread_t object.

co_thread_t * co_create(co_func_t func, uint32_t size, co_allocator_t *alloc);
```
Note: 

> Calling stdlib functions and OS apis can require a relatively large stack, and a reasonable choice in such situations is 512k to 1mb.  With care however vastly smaller stacks can be used.

> If nullptr is passed in as the alloc parameter then all memory operations will be handled via calls to new and delete[].

---
#### co_yield()
This function is the workhorse of the coroutine implementation.  This function will execute a thread context switch to which ever thread has been passed as an argument.  The currently executing thread will be halted and saved into the argument thread object, at the same time a thread from the co_thread_t argument will be unpacked and executed.

```
Arguments:
  thread - A coroutine thread instance to switch to.

void co_yield(co_thread_t *thread);
```
Note:
> Calling co_yield() on a thread instance which has previously exited will have no effect. 

> Yielding to one coroutine from from within another can have unsired consequences and should be avoided.  A coroutine should only yield to itself, to return control back to the main thread.

---
#### co_delete()
This function can be used to free a previously allocated coroutine instance.

```
Arguments:
  thread - A coroutine thread instance to be deleted.
  alloc  - A custom allocator to handle the delete operation (optional).

void co_delete(co_thread_t *thread, co_allocator_t *alloc);
```
Note:
> If nullptr is passed in as the alloc parameter then all memory operations will be handled via calls to new and delete[].

---
#### co_status()
It can be usefull to know the execution status of a coroutine thread.

```
Arguments:
  thread - A coroutine thread instance to query the status of.
  
Returns:
  COLIB_STATUS_ENDED, if the thread has exited.
  COLIB_STATUS_YIELDING, if the thread is a alive but is suspended.

int co_status(co_thread_t *thread);
```

---
#### co_set_user()
A co_thread_t can store an arbitary pointer to custom data.  The primaraly use for this mechanism is to allow custom arguments to be passed to a coroutine to control its execution.

```
Arguments:
  thread - A coroutine thread instance.
  data   - A pointer to be stored in the co_thread_t object.

void co_set_user(co_thread_t *thread, void *data);
```

---
#### co_get_user()
This is the dual function of co_set_user() as it will return any data that has previosly been associated with this co_thread_t instance.

```
Returns:
  A pointer that was previosly stored in this co_thread_t object.

void * co_get_user(co_thread_t *thread);
```

---
#### co_func_t
A coroutine thread function must have the prototype of a co_func_t.  When a co_routine begind execution of this function, its thread object is passed in as an argument.  To return to the main thread, a coroutine should call co_yield() with its own thread object.
```
typedef void (*co_func_t)(co_thread_t *self);
```
