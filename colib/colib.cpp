#include <string.h>
#include "coconfig.h"
#include "colib.h"

#define debug 0
#if debug
  #include <stdio.h>
#endif

// define our own assert since the stdlib one sucks
#if defined(_MSC_VER)
  #include <intrin.h>
  #define assert(X) {if (!(X)) __debugbreak();}
#else
  #include <assert.h>
#endif

// assembly provided routines
extern "C" void co_yield_asm(co_thread_t * self);
extern "C" void co_ret_asm();

// opaque co-routine structure
struct co_thread_t {
    uint8_t * sp_;
    uint8_t * stack_;
    uint32_t  size_;
    void    * user_;
};

namespace {

// this is the cookie that gets placed at the bottom of the stack to
// help us check for stack smashing.
const char co_guard[] = "COGUARD";

void * co_alloc(co_allocator_t *mem, uint32_t size) {
    if (mem)
        return mem->alloc_(size, mem->user_);
    else
        return new uint8_t[size];
}

void co_free(co_allocator_t *mem, void *alloc) {
    assert(alloc);
    if (mem)
        mem->free_ (alloc, mem->user_);
    else
        delete [] (uint8_t*)alloc;
}

// align the top of the stack as needed
uint8_t * co_align(uint8_t * p) {
    uint64_t shl = uint64_t (p) & 0x7;
    return p - shl;
}

// stack construction helper
template <typename type_t>
void push(uint8_t * & s, const type_t t) {
    s -= sizeof(type_t);
    memcpy(s, &t, sizeof(type_t));
#if debug
    printf ("0x%p %d ", s, int32_t(sizeof(t)));
    for (int i = 0; i < sizeof (t); i++)
        printf ("%02x ", ((uint8_t*)&t)[i]);
    printf ("\n");
#endif
}

// AMD64 ABI used by x64 Linux
// callee save: rbp, rbx, r12..r15
//
bool co_create_linux_x64(co_thread_t * t, co_func_t f, uint32_t size) {
    uint8_t * & rsp = t->sp_;
    // push the thread object
    push<co_thread_t*>(rsp, t);
    // return address for thread func
    push<void*>(rsp, (void*) co_ret_asm);
    // co-routine entry point
    push<void*>(rsp, (void*) f);
    // push dummy callee save registers
    for (uint32_t i=0; i<6; ++i)
        push<void*>(rsp, 0);
    return true;
}

// Win64 x64 ABI
// callee save: rbx, rbp, rdi, rsi, r12..r15
//
bool co_create_win_x64(co_thread_t * t, co_func_t f, uint32_t size) {
    uint8_t * & rsp = t->sp_;
    // push the thread object
    push<co_thread_t*>(rsp, t);
    // we must reserve 32bytes of space as 'shadow space'
    push<void*>(rsp, (void*)0);
    push<void*>(rsp, (void*)0);
    push<void*>(rsp, (void*)0);
    push<void*>(rsp, (void*)0);
    // return address for thread func
    push<void*>(rsp, (void*) co_ret_asm);
    // co-routine entry point
    push<void*>(rsp, (void*) f);
    // push dummy callee save registers
    for (uint32_t i=0; i<8; ++i)
        push<void*>(rsp, 0);
    return true;
}

// __cdecl x86 ABI
// callee save: esi, ebx, ebp, esi, edi
//
bool co_create_win_x86(co_thread_t * t, co_func_t f, uint32_t size) {
    uint8_t * & esp = t->sp_;
    // push the thread object
    push<co_thread_t*>(esp, t);
    // return address for thread func
    push<void*>(esp, (void*) co_ret_asm);
    // co-routine entry point
    push<void*>(esp, (void*) f);
    // push dummy callee save registers
    for (uint32_t i=0; i<5; ++i)
        push<void*>(esp, 0);
    return true;
}

// AMD64 ABI used by x64 Linux
// callee save: rbp, rbx, r12..r15
//
bool co_create_mac_x64(co_thread_t * t, co_func_t f, uint32_t size) {
    uint8_t * & rsp = t->sp_;
    // push the thread object
    push<co_thread_t*>(rsp, t);
    // return address for thread func
    push<void*>(rsp, (void*) co_ret_asm);
    // co-routine entry point
    push<void*>(rsp, (void*) f);
    // push dummy callee save registers
    for (uint32_t i=0; i<6; ++i)
        push<void*>(rsp, 0);
    return true;
}

// ARM V7 ABI
// callee save: r4..r11 (r9*)
// note: preserve the callee save VFPU registers?
//
bool co_create_linux_arm32(co_thread_t * t, co_func_t f, uint32_t size) {
    uint8_t * & rsp = t->sp_;
    // push the thread object
    push<co_thread_t*>(rsp, t);
    // return address for thread func
    push<void*>(rsp, (void*) co_ret_asm);
    // co-routine entry point
    push<void*>(rsp, (void*) f);
    // push dummy callee save registers
    for (uint32_t i=0; i<7; ++i)
        push<uint64_t>(rsp, 0);
    return false;
}

// MIPS32 ABI
// callee save: s0-s8
//
bool co_create_linux_mips32(co_thread_t * t, co_func_t f, uint32_t size) {
    uint8_t * & sp = t->sp_;
    // push the thread object
    push<co_thread_t*>(sp, t);
    return false;
}

// insert a cookie at the bottom of the stack
void co_insert_guard(co_thread_t * t) {
    assert(t);
    assert(t->stack_);
    memcpy(t->stack_, co_guard, sizeof(co_guard));
}

// check for a valid cookie in the stack
bool co_check_guard(co_thread_t * t) {
    assert(t);
    assert(t->stack_);
    return memcmp(co_guard, t->stack_, sizeof(co_guard)) == 0;
}

} // namespace {}

void co_yield(co_thread_t * self) {
    assert(self);
    if (self->sp_) {
        co_yield_asm(self);
        if (!co_check_guard(self)) {
            assert(!"stack smashed!");
        }
    }
}

co_thread_t * co_create(co_func_t f, uint32_t s, co_allocator_t *mem) {

    co_thread_t * t = (co_thread_t*)co_alloc(mem, sizeof (co_thread_t));
    if (!t)
        return nullptr;
    memset(t, 0, sizeof(co_thread_t));
    t->size_  = s;
    t->stack_ = (uint8_t*)co_alloc(mem, s);
    assert(t->stack_);
    memset(t->stack_, 0x13, s);
    t->sp_ = co_align(t->stack_ + s);
#if debug
    printf ("sp 0x%p\n", (void*)t->sp_);
#endif
    if (CO_CREATE(t, f, s)) {
        co_insert_guard(t);
        return t;
    }
    else {
        co_free(mem, t);
    }
    return nullptr;
}

void co_delete(co_thread_t *t, co_allocator_t *mem) {
    assert(t);
    if (t->stack_)
        co_free(mem, t->stack_);
    memset(t, 0, sizeof(co_thread_t));
    co_free(mem, t);
}

void co_set_user(co_thread_t * t, void *data) {
    assert(t);
    t->user_ = data;
}

void * co_get_user(co_thread_t * t) {
    assert(t);
    return t->user_;
}

int co_status(co_thread_t *t) {
    assert(t);
    if (t->sp_)
        return COLIB_STATUS_YIELDING;
    else
        return COLIB_STATUS_ENDED;
}
