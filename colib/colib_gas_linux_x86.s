# colib yield for linux x86 (intel x86 abi)
# GAS assembly

.global co_entry_asm
.global co_yield_asm
.global co_ret_asm

.text

# unused
co_entry_asm:
    ret

# void yield_( co_thread_t * thread )
#   thread pushed on stack
#
co_yield_asm:
    # pop thread object into eax
    movl %eax, 4(%esp)

    # push callee save registers
    push %esi
    push %ebx
    push %ebp
    push %esi
    push %edi

    movl 4(%eax), %edi
    movl %esp, (%edi)
    movl (%eax), %esp

    # pop callee save registers
    pop %edi
    pop %esi
    pop %ebp
    pop %ebx
    pop %esi

    # return to new thread
    ret

co_ret_asm:
    # pop the thread object into eax
    pop %eax

    #
    movl 4(%eax), %edi
    movl (%edi), %esp
    movl $0, (%eax)

    # pop callee save registers
    pop %edi
    pop %esi
    pop %ebp
    pop %ebx
    pop %esi

    # return to new thread
    ret
