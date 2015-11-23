.global co_entry_asm
.global co_yield_asm
.global co_ret_asm

.text
.align 2

#note: $s8 alias of $fp

co_entry_asm:
    la   $ra,  co_ret_asm
    lw   $t0,  0($sp)
    addi $sp,  $sp, -4
    jr   $t0
    add  $0, $0, $0

co_yield_asm:
    # push callee save registers
    addi $sp, $sp, -40
    sw   $s0,  0($sp)
    sw   $s1,  4($sp)
    sw   $s2,  8($sp)
    sw   $s3, 12($sp)
    sw   $s4, 16($sp)
    sw   $s5, 20($sp)
    sw   $s6, 24($sp)
    sw   $s7, 28($sp)
    sw   $s8, 32($sp)
    sw   $ra, 36($sp)
    # store current stack pointer
    lw   $t0,  4($a0)
    sw   $sp,  0($t0)
    # load new stack pointer
    lw   $sp,  0($a0)
    # load old callee save registers
    lw   $s0,  0($sp)
    lw   $s1,  4($sp)
    lw   $s2,  8($sp)
    lw   $s3, 12($sp)
    lw   $s4, 16($sp)
    lw   $s5, 20($sp)
    lw   $s6, 24($sp)
    lw   $s7, 28($sp)
    lw   $s8, 32($sp)
    lw   $ra, 36($sp)
    addi $sp, $sp, 40
    # return
    jr   $ra
    # fill delay slot
    add $0, $0, $0

co_ret_asm:

    # find thread object on stack
    lw   $t0,  0($sp)
    # load stack pointer
    lw   $t1,  4($t0)
    lw   $sp,  0($t1)
    # void old stack pointer
    sw   $0,   0($t0)
    # pop callee save registers
    lw   $s0,  0($sp)
    lw   $s1,  4($sp)
    lw   $s2,  8($sp)
    lw   $s3, 12($sp)
    lw   $s4, 16($sp)
    lw   $s5, 20($sp)
    lw   $s6, 24($sp)
    lw   $s7, 28($sp)
    lw   $s8, 32($sp)
    lw   $ra, 36($sp)
    addi $sp, $sp, 40
    # return
    jr  $ra
    # fill delay slot
    add $0, $0, $0
