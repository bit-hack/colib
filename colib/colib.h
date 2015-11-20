#pragma once
#include <stdint.h>

/* co_status() return codes
 */
#define COLIB_STATUS_ENDED     0
#define COLIB_STATUS_YIELDING  1

/* opaque co-routine thread struct
 */
struct co_thread_t;

/* custom memory allocator type for colib allocations
 */
struct co_allocator_t {
    void * (*alloc_)(uint32_t size, void * user);
    void   (*free_ )(void *mem, void * user);
    void * user_;
};

/* co-routine thread function prototype
 *
 * if a co-routine function returns then an automatic yield
 * will occur to the callee.  the status of the thread will also
 * be reported as 'exited'.
 */
typedef void (*co_func_t)(co_thread_t *self);

/* encapsulate the main thread
 *
 */
co_thread_t * co_init(co_allocator_t * alloc);

/* co_create
 *  create a co-routine thread
 *
 * params:
 *  self    - 
 *  func    - thread function to execute
 *  size    - size of the co-routine stack to allocate
 *  alloc   - custom memory allocator (can be nullptr)
 *  user    - user data for this coroutine
 *
 * returns:
 *  this function returns a co-routine thread instance, which can be resumed
 *  via co_yield().
 *
 *  if nullptr is passed in as the 'alloc' parameter, then all memory
 *  allocations will be dealt with via 'new' and 'delete'.
 */
co_thread_t * co_create(co_thread_t * self, 
                        co_func_t func, 
                        uint32_t size, 
                        co_allocator_t *alloc=nullptr, 
                        void * user=nullptr);

/* co_yield
 *  yield to the thread stored in the co_thread_t struct
 *
 * params:
 *  self  - the thread instance that is yielding
 *  to    - the thread that will begin executing
 *
 * note:
 *  if 'to' is nullptr then this thread will yield to the previous thread
 */
void co_yield(co_thread_t * self, 
              co_thread_t * to = nullptr);

/* yield the current co-routine to the main thread
 *
 */
void co_yield_to_main(co_thread_t * self);

/* co_delete
 *  delete a co-routine instance
 *
 * params:
 *  thread  - a co-routine thread instance
 */
void co_delete(co_thread_t *thread);

/* co_status
 *  get the execution status of a co-routine
 *
 * params:
 *  thread  - a co-routine thread instance
 *
 * returns:
 *  COLIB_THREAD_ENDED    - co-routine has exited
 *  COLIB_THREAD_YIELDING - co-routine is suspended
 */
int co_status(co_thread_t *thread);

/* co_set_user
 *  set the user field of a co_thread_t instance.
 *
 * params:
 *  thread  - a co-routine thread instance
 *  data    - a (void*) to be written to the user field
 *
 * note:
 *  the primary purpose of the user field is to allow arbitrary data
 *  to be passed between a co-routine and its caller.  typically set by the
 *  main thread and accessed for use in the co-routine.
 */
void co_set_user(co_thread_t *thread, 
                 void *data);

/* co_get_user 
 *  retrive the user field of a co_thread_t instance.
 *
 * params:
 *  thread  - a co-routine thread instance
 *
 * returns:
 *  the (void*) that had previously been set via co_set_user.
 */
void * co_get_user(co_thread_t *thread);
