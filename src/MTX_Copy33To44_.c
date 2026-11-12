// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive: block copy,
// CP15, IRQ/CPSR, svc, matrix/math, context-switch). No C to decompile to. Per asm policy.
// NitroSDK fx_mtx33.c: copies the 3x3 src into the upper-left of the 4x4 dst and
// fills the rest of dst from the identity.
#ifndef SM64DS_PLATFORM_PC
asm void MTX_Copy33To44_(void *src, void *dst) {
    stmdb sp!, {r4}
    mov ip, #0
    ldmia r0!, {r2, r3, r4}
    stmia r1!, {r2, r3, r4, ip}
    ldmia r0!, {r2, r3, r4}
    stmia r1!, {r2, r3, r4, ip}
    ldmia r0!, {r2, r3, r4}
    stmia r1!, {r2, r3, r4, ip}
    mov r2, #0x1000
    str ip, [r1]
    str ip, [r1, #4]
    str ip, [r1, #8]
    str r2, [r1, #0xc]
    ldmia sp!, {r4}
    bx lr
}
#else
void MTX_Copy33To44_(void *src, void *dst);
#endif
