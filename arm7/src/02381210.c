// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02381210 size 0x12C (runtime 0x037F90A8, wram)
// C tried by: AR47; asm taken from the AR47 branch.
/* storage 0x02381210 runtime 0x037F90A8 size 0x12C wram. */
/* 2004 NitroSDK thread-init, hand-written asm like its OS_context siblings:
 * b56 provably folds the LDR=0x400 test and the duplicated 0x0380FF80-0x200
 * subtract in any C spelling (see AR47 report), so this is transcribed as asm.
 * Pool order matches retail; address pools are relocs, the rest numeric. */

extern int dword_3806E5C;
extern int word_3806E60;
extern int dword_3806E68;
extern int dword_3806E58;
extern int unk_3806F2C;
extern void sub_37F8000(void);
extern int fn_023A0370(int a1);

asm int fn_02381210(register int a1)
{
    stmdb sp!, {lr}
    sub sp, sp, #4
    ldr r0, =dword_3806E5C
    ldr r1, [r0]
    cmp r1, #0
    bne done
    mov r1, #1
    str r1, [r0]
    mov r1, #0
    mov r3, r1
    ldr r2, =word_3806E60
loop_zero:
    add r0, r2, r1, lsl #2
    str r3, [r0, #0x14]
    add r1, r1, #1
    cmp r1, #0x10
    blt loop_zero
    ldr r1, =dword_3806E68
    ldr r0, =dword_3806E58
    str r1, [r0]
    mov r1, #0x10
    ldr r0, =unk_3806F2C
    str r1, [r0, #0x54]
    str r3, [r0, #0x50]
    mov r1, #1
    str r1, [r0, #0x48]
    str r3, [r0, #0x4c]
    str r3, [r0, #0x58]
    str r0, [r2, #0x14]
    str r0, [r2, #0xc]
    str r0, [r2, #0x8]
    ldr r2, =0x400
    cmp r2, #0
    ldreq r0, =sub_37F8000
    subeq r2, r0, r2
    ldrne r1, =0x200
    ldrne r0, =0x380FF80
    subne r0, r0, r1
    subne r2, r0, r2
    ldr r1, =0x200
    ldr r0, =0x380FF80
    sub r3, r0, r1
    ldr r1, =unk_3806F2C
    str r3, [r1, #0x6c]
    str r2, [r1, #0x68]
    mov r0, #0
    str r0, [r1, #0x70]
    ldr r2, =0xD73BFDF7
    str r2, [r3, #-4]
    ldr r3, =0xFBDD37BB
    ldr r2, [r1, #0x68]
    str r3, [r2]
    strh r0, [r1, #0x74]
    mov r2, #0x10
    ldr r1, =word_3806E60
    strh r2, [r1, #2]
    strh r0, [r1]
    strh r0, [r1, #4]
    ldr r2, =0x27FFFA4
    str r1, [r2]
    bl fn_023A0370
done:
    add sp, sp, #4
    ldmia sp!, {lr}
    bx lr
}
