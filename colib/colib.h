/* COLIB
 * Tiny, hackable, cross platform coroutine library.
 * By Aidan Dodds, 2015
 */
#pragma once
#include <stdint.h>

/** \brief Return code from co_status().
 *
 * Signals that this coroutine has returned and can not be resumed.
 */
#define COLIB_STATUS_ENDED     0

/** \brief Return code from co_status().
 *
 * Signals that this coroutine has yielded and may resume execution.
 */
#define COLIB_STATUS_YIELDING  1

/** \brief The struct that encapsulates a coroutine thread.
 *
 * This structure is opaque and defined in colib.cpp.  It is not safe to modify
 * its contents directly.
 */
struct co_thread_t;

/** \brief Custom memory allocation struct.
 *
 * A custom memory allocator can be passed into colib functions to handle all
 * memory allocations and deallocations.  A nullptr can be passed instead
 * so that colib will use new/delete internally.
 */
struct co_allocator_t {
    void * (*alloc_)(uint32_t size, void * user);
    void   (*free_ )(void *mem, void * user);
    void * user_;
};

/** \brief The coroutine entry point prototype.
 *
 * A coroutine is specified as a C style function and must have the following
 * prototype.  When an coroutine is created via co_create() its entry point
 * must be given.  On entry, a coroutine is passed its own thread object.
 *
 * If a co-routine function returns then an automatic yield will occur to the
 * previously executing thread.  the status of the thread will also report as 
 * COLIB_STATUS_ENDED.  A coroutine which has ended can not be resumed.
 *
 * \param self The thread object encapsulating the current coroutine.
 */
typedef void (*co_func_t)(co_thread_t *self);

/** \brief Get the main thread as a coroutine.
 *
 * This function encapsulates the main thread as a coroutine thread object.
 * This allows the main thread to yield to coroutines, and must be called prior
 * to any other colib functions.
 *
 * \param alloc The custom memory allocator to use for all memory allocations.
 * Set this parameter to nullptr to use new and delete internally.
 */
co_thread_t * co_init(co_allocator_t * alloc);

/** \brief Create a coroutine.
 *
 * Create a new coroutine with a given entry point and stack size.
 *
 * Note: If you plan to call any OS or library functions then you must ensure
 * that the stack is large enough to accommodate that.
 *
 * \param self The currently executing coroutine or host thread.
 * \param func The coroutine entry point function.
 * \param size The size of the coroutine stack.
 * \param alloc The custom allocator to use (may be nullptr).
 * \param user User data pointer.
 */
co_thread_t * co_create(co_thread_t * self, 
                        co_func_t func, 
                        uint32_t size, 
                        co_allocator_t *alloc=0,
                        void * user=0);

/** \brief Context switch to a different coroutine.
 *
 * Suspend the current thread and pass execution to the specified coroutine.
 * If no target coroutine is specified, execution is passed to the previous
 * coroutine.
 *
 * \param self The current executing coroutine.
 * \param to The thread to yield to. If nullptr, then this call will yield to
 * the previously executing coroutine.
 */
void co_yield(co_thread_t * self, 
              co_thread_t * to=0);

/** \brief Context switch to the main thread.
 *
 * Suspend the current coroutine and pass execution back to the main (host)
 * thread.  The main thread has previously been returned by co_init(), and
 * used for root co_create calls().
 *
 * \param self The currently executing coroutine.
 */
void co_yield_to_main(co_thread_t * self);

/** \brief Delete a coroutine.
 *
 * All memory deletions will be handled via the allocator that was used to
 * create the coroutine.  If nullptr was passed, then this will be handled
 * internaly via delete.
 *
 * \param The coroutine or host thread object to delete.
 */
void co_delete(co_thread_t *thread);

/** \brief Return the execution status of a coroutine.
 *
 * \param thread The coroutine or host thread object to query the status of.
 */
int co_status(co_thread_t *thread);

/** \brief Set the user data of a coroutine.
 *
 * This function sets the user data pointer in the coroutine thread object.
 * The primary purpose of the user field is to allow arbitrary data to be
 * passed between a co-routine and its caller.
 *
 * \param thread The coroutine or host thread object to set the user data of.
 * \param data A pointer to the user data to be stored in this thread object.
 */
void co_set_user(co_thread_t *thread, 
                 void *data);

/** \brief Get the user data of a coroutine.
 *
 * This function returns the user data pointer for a coroutine thread object.
 * The primary purpose of the user field is to allow arbitrary data to be
 * passed between a co-routine and its caller.
 *
 * \param thread The coroutine or host thread object to return the user data
 * pointer from.
 */
void * co_get_user(co_thread_t *thread);
