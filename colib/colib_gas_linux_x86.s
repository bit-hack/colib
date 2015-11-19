# colib yield for linux x64 (AMD64 ABI)
# GAS assembly

.global co_yield_asm
.global co_ret_asm

.text

# void yield_( co_thread_t * thread )
#   thread pushed on stack
#
co_yield_asm:
    # pop thread into eax
    mov %eax, 4(%esp)
    # push callee save registers
    push %esi
    push %ebx
    push %ebp
    push %esi
    push %edi
    # swap( thread->sp_, esp )
    xchg (%eax), %esp
    # pop callee save registers
    pop %edi
    pop %esi
    pop %ebp
    pop %ebx
    pop %esi
    # return to new thread
    ret

# when a coroutine function returns, it will execute this function
# since it was pushed as the return address on the stack.  below this
# address the coroutine thread object has been pushed.  we pop that
# from the stack and switch contexts, back to the main thread.
co_ret_asm:
    # pop the thread object into eax
    pop %eax
    # swap( thread->sp_, esp )
    xchg %esp, (%eax)
    # thread->sp_ = nullptr
    movd $0, (%eax)
    # pop callee save registers
    pop %edi
    pop %esi
    pop %ebp
    pop %ebx
    pop %esi
    # return to new thread
    ret
