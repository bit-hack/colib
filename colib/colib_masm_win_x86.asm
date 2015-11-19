; colib yield for windows x86 __cdecl
; MASM assembly

.386
.MODEL FLAT,C
.CODE

; void yield_( co_thread_t * thread )
;   thread pushed on stack
;
co_yield_asm PROC
    ; pop thread into eax
    mov eax, [esp+4]
    ; push callee save registers
    push esi
    push ebx
    push ebp
    push esi
    push edi
    ; swap( thread->sp_, esp )
    xchg [eax], esp
    ; pop callee save registers
    pop edi
    pop esi
    pop ebp
    pop ebx
    pop esi
    ; return to new thread
    ret
co_yield_asm ENDP

; when a coroutine function returns, it will execute this function
; since it was pushed as the return address on the stack.  below this
; address the coroutine thread object has been pushed.  we pop that
; from the stack and switch contexts, back to the main thread.
co_ret_asm PROC
    ; pop the thread object into eax
    pop eax
    ; swap( thread->sp_, esp )
    xchg [eax], esp
    ; thread->sp_ = nullptr
    mov DWORD PTR [eax+0], 0
    ; pop callee save registers
    pop edi
    pop esi
    pop ebp
    pop ebx
    pop esi
    ; return to new thread
    ret
co_ret_asm ENDP
END
