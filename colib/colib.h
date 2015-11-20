#pragma once
#include <stdint.h>

/* \brief Return codes for co_status().
 */
#define COLIB_STATUS_ENDED     0
#define COLIB_STATUS_YIELDING  1

/* \brief The struct that encapsulates a coroutine thread.
 */
struct co_thread_t;

/** \brief Custom memory allocation struct.
 */
struct co_allocator_t {
    void * (*alloc_)(uint32_t size, void * user);
    void   (*free_ )(void *mem, void * user);
    void * user_;
};

/** \brief A coroutine prototype.
 *
 * A coroutine is specified as a C style function and must have the following
 * prototype.
 *
 * If a co-routine function returns then an automatic yield will occur to the
 * previously executing thread.  the status of the thread will also report as 
 * COLIB_STATUS_ENDED.
 *
 * \param self The thread object encapsulating the current coroutine.
 */
typedef void (*co_func_t)(co_thread_t *self);

/** \brief Get the main thread as a coroutine.
 *
 * This function encapsulates the main thread in a thread object.  This allows
 * the main thread to yield to coroutines, and must be called before any other
 * colib functions.
 *
 * \param alloc The custom memory allocator to use for all memory allocations.
 * Set this parameter to nullptr to use new and delete internally.
 */
co_thread_t * co_init(co_allocator_t * alloc);

/** \brief Create a coroutine.
 *
 */
co_thread_t * co_create(co_thread_t * self, 
                        co_func_t func, 
                        uint32_t size, 
                        co_allocator_t *alloc=nullptr, 
                        void * user=nullptr);

/** \brief Context switch to a different coroutine.
 */
void co_yield(co_thread_t * self, 
              co_thread_t * to = nullptr);

/** \brief Context switch to the main thread.
 *
 */
void co_yield_to_main(co_thread_t * self);

/** \brief Delete a coroutine.
 */
void co_delete(co_thread_t *thread);

/** \brief Return the execution status of a coroutine.
 */
int co_status(co_thread_t *thread);

/** \brief Set the user data of a coroutine.
 *
 * The primary purpose of the user field is to allow arbitrary data to be
 * passed between a co-routine and its caller.
 */
void co_set_user(co_thread_t *thread, 
                 void *data);

/** \brief Get the user data of a coroutine.
 *
 */
void * co_get_user(co_thread_t *thread);
