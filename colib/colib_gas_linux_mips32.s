# colib yield for linux mips32
# GAS assembly

.global co_yield_asm
.global co_ret_asm

.text

# arguments passed in:  a0-a3
# callee save:          s0-s7, s8
# return address:       ra

# void (co_yield_asm)( co_thread_t * thread );
#   thread -> a0
#
co_yield_asm:
    # save all callee save registers
    push %s0
    push %s1
    push %s2
    push %s3
    push %s4
    push %s5
    push %s6
    push %s7
    push %s8
    push %ra
    # swap( rsp, thread->rsp_ )
    xchg 0(%a0), %sp
    # pop all callee save register
    pop %ra
    pop %s8
    pop %s7
    pop %s6
    pop %s5
    pop %s4
    pop %s3
    pop %s2
    pop %s1
    pop %s0
    # return to other thread
    ret

# when the thread function returns it will return into
# this function which will execute an implicit yield.
# we also clear the rsp_ field to signal this thread has
# terminated.
#
co_ret_asm:
    # pop the thread object into rdi
    mov %v0, 40(%sp)
    # swap( rsp, thread->rsp_ )
    xchg 0(%v0), %sp
    # thread->rsp_ = nullptr
    mov $0, 0(%rsp)
    # pop all callee save register
    pop %ra
    pop %s8
    pop %s7
    pop %s6
    pop %s5
    pop %s4
    pop %s3
    pop %s2
    pop %s1
    pop %s0
    # return to other thread
    ret
