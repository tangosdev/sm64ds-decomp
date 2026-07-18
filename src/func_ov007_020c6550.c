/* Byte-identical via mwccarm asm: C always mis-colors the [c+8]/[c+0xc]
 * compare (r1/r2 swap vs load-order deadlock with volatile). r0 still holds
 * `c` at the first bl (no rematerialize). Verified match.py 1.2/sp2p3. */
extern void func_ov007_020c6d20(void);
extern void func_ov007_020c28ac(void);

asm int func_ov007_020c6550(register char* c)
{
    stmdb sp!, {r4, r5, r6, lr}
    mov r6, r0
    ldr r4, [r6, #0x20]
    mov r5, #1
    ldrh r1, [r4, #8]
    cmp r1, #0
    beq Lfail
    ldr r1, [r6, #8]
    ldr r2, [r6, #0xc]
    cmp r1, r2
    blt Lsucc
Lfail:
    mov r5, #0
    b Lend
Lsucc:
    bl func_ov007_020c6d20
    add r1, r6, #8
    ldr r0, [r1]
    add r0, r0, #1
    str r0, [r1]
    ldr r1, [r6, #8]
    ldr r0, [r6, #0xc]
    cmp r1, r0
    mov r0, r4
    movge r5, #2
    bl func_ov007_020c28ac
Lend:
    mov r0, r5
    ldmia sp!, {r4, r5, r6, lr}
    bx lr
}
