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
extern "C" void co_entry_asm();
extern "C" void co_yield_asm(co_thread_t * self);
extern "C" void co_ret_asm();

// opaque co-routine structure
struct co_thread_t {

    // this threads stack pointer
    uint8_t * sp_;

    // the thread that yielded to us
    co_thread_t * callee_;

    // the base allocation of this stack
    uint8_t * stack_;
    uint32_t  size_;

    // this threads user data
    void * user_;

    // the main thread
    co_thread_t * main_;

    // the allocator that created this thread
    co_allocator_t * alloc_;
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
    uint64_t shl = uint64_t (p) & 0xf;
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

// AMD64 ABI used by x64 posix
// callee save: rbp, rbx, r12..r15
//
bool co_create_posix_x64(co_thread_t * t, co_func_t f, uint32_t size) {
    uint8_t * & rsp = t->sp_;
    // push padding to realign the stack pointer
    for (uint32_t i = 0; i < 7; ++i)
        push<void*>(rsp, (void*)0);
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
    // push padding to realign the stack pointer
    for (uint32_t i = 0; i < 1; ++i)
        push<void*>(rsp, (void*)0);
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
bool co_create_generic_x86(co_thread_t * t, co_func_t f, uint32_t size) {
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

// ARM V7 ABI
// callee save: r4..r11, lr
//
bool co_create_linux_arm(co_thread_t * t, co_func_t f, uint32_t size) {
    uint8_t * & rsp = t->sp_;
    // push the thread object
    push<co_thread_t*>(rsp, t);
    // return address for thread func
    push<void*>(rsp, (void*) f);
    // co-routine entry point
    push<void*>(rsp, (void*) co_entry_asm);
    // push dummy callee save registers
    for (uint32_t i=0; i<8; ++i)
        push<uint32_t>(rsp, 0xabcdef00 + i);
    return true;
}

// MIPS32 ABI
// callee save: s0-s8, fp
//
bool co_create_linux_mips(co_thread_t * t, co_func_t f, uint32_t size) {
    uint8_t * & sp = t->sp_;
    // push the thread object
    push<co_thread_t*>(sp, t);
    // push co-routine entry point
    push<void*>(sp, (void*) f);
    // push co-routine entry trampoline
    push<void*>(sp, (void*) co_entry_asm);
    // push dummy callee save registers
    for (uint32_t i=0; i<9; ++i)
        push<uint32_t>(sp, 0xaabbcc00+i);
    return true;
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
    // dont stack check the main thread
    if (t == t->main_)
        return true;
    assert(t->stack_);
    return memcmp(co_guard, t->stack_, sizeof(co_guard)) == 0;
}

} // namespace {}

void co_yield(co_thread_t * self, co_thread_t * to) {
    assert(self);

    if (self == to)
        return;

    if (!to)
        to = self->callee_;
    assert(to);
    to->callee_ = self;

    if (to->sp_) {
        co_yield_asm(to);
        if (!co_check_guard(to)) {
            assert(!"stack smashed!");
        }
    }
}

co_thread_t * co_create(co_thread_t * self, co_func_t f, uint32_t s, co_allocator_t *mem, void * user) {
    co_thread_t * t = (co_thread_t*)co_alloc(mem, sizeof (co_thread_t));
    if (!t)
        return nullptr;
    memset(t, 0, sizeof(co_thread_t));
    t->alloc_  = mem;
    t->main_   = self->main_;
    t->callee_ = nullptr;
    t->callee_ = nullptr;
    t->size_   = s;
    t->stack_  = (uint8_t*)co_alloc(mem, s);
    t->sp_     = co_align(t->stack_ + s);
    t->user_   = user;
    assert(t->stack_);
    memset(t->stack_, 0x13, s);
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

void co_delete(co_thread_t *t) {
    assert(t);
    if (t != t->main_ && t->stack_)
        co_free(t->alloc_, t->stack_);
    co_free(t->alloc_, t);
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

co_thread_t * co_init(co_allocator_t * mem) {
    co_thread_t * t = (co_thread_t*) co_alloc(mem, sizeof(co_thread_t));
    memset(t, 0, sizeof(co_thread_t));
    t->alloc_  = mem;
    t->callee_ = nullptr;
    t->main_   = t;
    t->size_   = ~0u;
    t->sp_     = nullptr;
    t->user_   = 0;
    return t;
}

void co_yield_to_main(co_thread_t * self) {
    co_yield(self, self->main_);
}
