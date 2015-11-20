; colib yield for windows x86 __cdecl
; MASM assembly

.386
.MODEL FLAT,C
.CODE

; void yield_( co_thread_t * thread )
;   thread pushed on stack
;
co_yield_asm PROC
    ; pop thread object into eax
    mov eax, [esp+4]

    ; push callee save registers
    push esi
    push ebx
    push ebp
    push esi
    push edi

    mov edi, [eax+4]
    mov [edi], esp

    mov esp, [eax]

    ; pop callee save registers
    pop edi
    pop esi
    pop ebp
    pop ebx
    pop esi

    ; return to new thread
    ret
co_yield_asm ENDP


co_ret_asm PROC
    ; pop the thread object into eax
    pop eax

    ; switch stack to callee coroutine
    mov edi, [eax+4]
    mov esp, [edi]

    ; nullify current coroutine stack pointer
    mov DWORD PTR [eax], 0

    ; pop callee save registers
    pop edi
    pop esi
    pop ebp
    pop ebx
    pop esi

    ; return to callee
    ret
co_ret_asm ENDP

END
