; colib yield for windows x64 ABI
; MASM assembly

.CODE

; unused
co_entry_asm PROC
    ret
co_entry_asm ENDP

; void yield_( co_thread_t * thread )
;   thread -> rcx
;
; note: we dont touch the RCX register where 'thread' was passed
;       so that it becomes a parameter to the thread function.
;
co_yield_asm PROC
;    int 3
    ; push callee save registers
    push RBX
    push RBP
    push RDI
    push RSI
    push R12
    push R13
    push R14
    push R15
    ; thread->callee_->sp_ = RSP
    mov QWORD PTR rax, [rcx+8]
    mov QWORD PTR [rax], rsp
    ; RSP = thread->sp_
    mov QWORD PTR rsp, [rcx]
    ; pop callee save registers
    pop R15
    pop R14
    pop R13
    pop R12
    pop RSI
    pop RDI
    pop RBP
    pop RBX
    ; return to new thread
    ret
co_yield_asm ENDP

; when a coroutine function returns, it will execute this function
; since it was pushed as the return address on the stack.  below this
; address the coroutine thread object has been pushed.  we pop that
; from the stack and switch contexts, back to the previous thread.
co_ret_asm PROC
;    int 3
    ; we move our thread object into rcx
    ; we add 32 here to step over the 'shadow space'
    mov rcx, [rsp+32]
    ; RSP = thread->callee_->sp_
    mov QWORD PTR rax, [rcx+8]
    mov QWORD PTR rsp, [rax]
    ; thread->sp_ = nullptr
    mov QWORD PTR [rcx], 0
    ; pop callee save registers
    pop R15
    pop R14
    pop R13
    pop R12
    pop RSI
    pop RDI
    pop RBP
    pop RBX
    ; return to main thread
    ret
co_ret_asm ENDP

END
