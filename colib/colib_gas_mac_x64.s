# colib yield for linux x64 (AMD64 ABI)
# GAS assembly

.global _co_yield_asm
.global _co_ret_asm

.text

# void (co_yield_asm)( co_thread_t * thread );
#   thread -> rdi
#
_co_yield_asm:
    # save all callee save registers
    push %rbp
    push %rbx
    push %r12
    push %r13
    push %r14
    push %r15
    # swap( rsp, thread->rsp_ )
    xchg 0(%rdi), %rsp
    # pop all callee save register
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbx
    pop %rbp
    # return to other thread
    ret

# when the thread function returns it will return into
# this function which will execute an implicit yield.
# we also clear the rsp_ field to signal this thread has
# terminated.
#
_co_ret_asm:
    # pop the thread object into rdi
    pop %rdi
    # swap( rsp, thread->rsp_ )
    xchg 0(%rdi), %rsp
    # thread->rsp_ = nullptr
    movq $0, (%rdi)
    # pop all callee save register
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbx
    pop %rbp
    # return to other thread
    ret
