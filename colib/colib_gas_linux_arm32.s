# colib yield for linux ARM32 (ARMEABI-V7A)
# GAS assembly

# callee save:  r4, r5, r6, r7, r8, (r9), r10, r11

.global co_yield_asm
.global co_ret_asm

.text

# void (co_yield_asm)( co_thread_t * thread );
#   thread -> r0
#
co_yield_asm:
    # save all callee save registers
    PUSH {lr,  r4}
    PUSH {r5,  r6}
    PUSH {r7,  r8}
    PUSH {r9,  r10}
    PUSH {r11, v1}
    # swap( rsp, thread->rsp_ )
    mov v1, sp
    mov v1, 
    # pop all callee save register
    POP {r11, v1}
    POP {r10, r9}
    POP {r8,  r7}
    POP {r6,  r5}
    POP {r4,  lr}
    # return to other thread
    bx lr

# when the thread function returns it will return into
# this function which will execute an implicit yield.
# we also clear the rsp_ field to signal this thread has
# terminated.
#
co_ret_asm:
    # return to other thread
    RET lr
