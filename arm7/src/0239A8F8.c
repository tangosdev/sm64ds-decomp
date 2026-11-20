// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x0239A8F8 size 0x74 (runtime 0x027CC870, main)
// C tried by: the AC C-only redo round; no C attempt reproduced the bytes (see the notes below).
/* Hand-asm: conditional-store pair (strhne/strheq) and a dead 4-byte
   stack adjustment are unreachable from C.
   Pool: 0x380FFF4, 0x048080AC, 0x048080AE. */
extern int *dat_0380FFF4;

asm void fn_0239A8F8(int a1, int a2)
{
    stmfd sp!, {lr}
    sub sp, sp, #4
    ldr lr, =dat_0380FFF4
    ldr r2, [lr]
    add r2, r2, #0x400
    ldrh ip, [r2, #0xEA]
    mov r3, #1
    mvn r3, r3, lsl r0
    and r3, ip, r3
    orr r0, r3, r1, lsl r0
    strh r0, [r2, #0xEA]
    ldr r0, [lr]
    add r0, r0, #0x400
    ldrh r1, [r0, #0xEA]
    ldrh r0, [r0, #0xEE]
    mvn r0, r0
    ands r0, r1, r0
    movne r1, #8
    ldrne r0, =0x48080AC
    strneh r1, [r0]
    moveq r1, #8
    ldreq r0, =0x48080AE
    streqh r1, [r0]
    add sp, sp, #4
    ldmfd sp!, {lr}
    bx lr
}
