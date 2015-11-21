# colib yield for osx x64 (AMD64 ABI)
# GAS assembly

.global _co_yield_asm
.global _co_ret_asm

.text

# void (co_yield_asm)( co_thread_t * thread );
_co_yield_asm:
    push %rbp
    push %rbx
    push %r12
    push %r13
    push %r14
    push %r15
    movq 8(%rdi), %rdx
    movq %rsp, 0(%rdx)
    movq 0(%rdi), %rsp
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbx
    pop %rbp
    ret

_co_ret_asm:
    pop %rdi
    movq 8(%rdi), %rdx
    movq 0(%rdx), %rsp
    movq $0, (%rdi)
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbx
    pop %rbp
    ret
