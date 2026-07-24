// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
/* func_02068398 @ 0x02068398 size 0x78 — arm9
 * Double-guard predication floor from plain C: two sequential null checks that both
 * branch to a shared tail with a 1-instr reassignment (mov r1,r0) between them.
 * mwccarm cond-opt collapses the trailing guard to movne; the c048 2-pred join costs
 * extra bytes. Hand-asm matches: ldmeqia (not ldmeq alone) spells the ROM's
 * predicated early-return (ldmeq sp!,{lr}; bxeq lr).
 * mwccarm 1.2/sp2p3: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e
 */
extern unsigned char data_020a9d2c;
extern void func_02068484(void);

asm void func_02068398(void)
{
    stmdb sp!, {lr}
    sub sp, sp, #4
    ldr r0, =data_020a9d2c
    ldr r1, [r0]
    cmp r1, #0
    addeq sp, sp, #4
    moveq r0, #0
    ldmeqia sp!, {lr}
    bxeq lr
    ldr r0, [r0, #4]
    cmp r0, #0
    beq L_done
    ldr r0, [r0, #0x4b4]
    cmp r0, #0
    beq L_done
    mov r1, r0
L_done:
    ldr r0, =data_020a9d2c
    str r1, [r0, #4]
    bl func_02068484
    ldr r1, =data_020a9d2c
    mov r2, #2
    ldr r3, [r1, #4]
    mov r0, #1
    ldrb r3, [r3, #0x4ac]
    strb r3, [r1, #0xe]
    strb r2, [r1, #0xc]
    add sp, sp, #4
    ldmia sp!, {lr}
    bx lr
}
