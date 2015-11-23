.global co_entry_asm
.global co_yield_asm
.global co_ret_asm

.text
.align 8

co_entry_asm:
    # note: when we enter here r0 still contains the
    #       thread object.
    # pop entry point off stack
    pop {r1}
    # setup link register
    ldr lr, =co_ret_asm
    # return into the coroutine
    mov pc, r1

co_yield_asm:
    # push callee save registers
    push {v1-v8, lr}
    # pop thread object into eax
    ldr r1, [r0, #4]
    str sp, [r1]
    ldr sp, [r0]
    # pop callee save registers
    pop {v1-v8, lr}
    # return to new thread
    mov pc, lr

co_ret_asm:
    # context switch to previous thread
    ldr r0, [sp]
    ldr r1, [r0, #4]
    ldr sp, [r1]
    # zero out the old sp
    mov r1, #0
    str r1, [r0]
    # pop callee save registers
    pop {v1-v8, lr}
    # return to new thread
    mov pc, lr
