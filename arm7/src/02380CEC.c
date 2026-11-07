// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02380CEC size 0x80 (runtime 0x037F8B84, wram)
// C tried by: the AC C-only redo round; no C attempt reproduced the bytes (see the notes below).
/* storage 0x02380CEC runtime 0x037F8B84 size 0x70 wram. Hand-asm: the
   0x27FFFF0 base rides caller-saved R2 across the BL loop, which mwccarm
   never emits for C. Pool +0x70: dat_03806E50. */

extern int dat_03806E50;
extern void fn_02380B14(void);  /* sub_37F89AC delay helper */

asm int fn_02380CEC(void) {
    stmfd sp!, {lr}
    sub sp, sp, #4
    ldr r0, =dat_03806E50
    ldr r1, [r0]
    cmp r1, #0
    bne out
    mov r1, #1
    str r1, [r0]
    mov r0, #0
    ldr r2, =0x27FFFF0
    strh r0, [r2, #6]
    b test
loop:
    bl fn_02380B14
test:
    ldrh r0, [r2, #4]
    cmp r0, #0x7F
    bne loop
    mvn r1, #0
    ldr r0, =0x27FFFB8
    str r1, [r0]
    mov r0, #0x10000
    rsb r1, r0, #0
    ldr r0, =0x27FFFBC
    str r1, [r0]
    mov r0, #0xBF
    strh r0, [r2, #6]
out:
    add sp, sp, #4
    ldmfd sp!, {lr}
    bx lr
}
