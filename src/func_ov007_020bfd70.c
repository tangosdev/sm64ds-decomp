// HAND-ASM: pure regperm floor on 7 words under mwccarm 1.2/sp2p3 C.
// Structure/schedule matches C tip; only table/sin_i/sin_byte/#0x800 colors
// differ (ip/r2/lr/r0 vs r0/ip/ip/r1). Exhausted 100+ C variants + permuter
// 1700+ iters score floor 45. Same class as func_02058568 (regperm → asm hatch).
extern void MulVec3Mat4x3(void* v, void* m, void* out);
extern int _ZN4cstd4fdivEii(int a, int b);
extern short data_02082214[];

asm void func_ov007_020bfd70(void)
{
    stmdb sp!, {r4, r5, r6, lr}
    sub sp, sp, #0x10
    mov r6, r0
    mov r5, r2
    mov r0, r1
    add r2, sp, #0
    add r1, r6, #0x44
    mov r4, r3
    bl MulVec3Mat4x3
    ldrh r1, [r6, #0xd4]
    ldr r3, [sp, #8]
    ldr ip, [pc, #0xa4]
    mov r1, r1, asr #4
    mov r2, r1, lsl #1
    mov r1, r2, lsl #1
    cmp r3, #0
    add r2, r2, #1
    ldrsh r1, [ip, r1]
    rsblt r3, r3, #0
    mov lr, r2, lsl #1
    smull r3, r2, r1, r3
    mov r0, #0x800
    adds r1, r3, r0
    ldrsh r0, [ip, lr]
    adc r2, r2, #0
    mov r1, r1, lsr #0xc
    orr r1, r1, r2, lsl #20
    bl _ZN4cstd4fdivEii
    ldr r1, [sp, #4]
    ldr r2, [sp]
    smull r3, ip, r0, r1
    mov r1, #0x800
    adds lr, r3, r1
    smull r3, r2, r0, r2
    adc r0, ip, #0
    adds r1, r3, r1
    mov r3, lr, lsr #0xc
    orr r3, r3, r0, lsl #20
    adc r0, r2, #0
    mov r1, r1, lsr #0xc
    orr r1, r1, r0, lsl #20
    mov r0, #0x60
    mul r2, r1, r0
    rsb r1, r3, #0
    mul r0, r1, r0
    mov r1, r2, asr #0xc
    add r1, r1, #0x80
    mov r0, r0, asr #0xc
    str r1, [r5]
    add r0, r0, #0x60
    str r0, [r4]
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, lr}
    bx lr
    dcd data_02082214
}
