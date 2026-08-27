// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only runtime primitive).
// CPU exception/abort dispatcher: reads the processor mode out of CPSR (mrs r1,cpsr; tests
// 0x17 abort / 0x1b undefined-instruction mode), manually saves context with stmdb and
// repoints sp, and spin-loops to halt. None of that (CPSR mode read, manual sp management,
// the halt spin) is expressible in C under our flags. Per asm policy.
extern void func_02059468(void);

extern void *data_020a63a8;

asm void func_020593f4(void)
{
    ldr     ip, =data_020a63a8
    ldr     ip, [ip]
    cmp     ip, #0
    movne   lr, pc
    bxne    ip
    ldr     ip, =0x2000000
    stmdb   ip!, {r0, r1, r2, r3, sp, lr}
    and     r0, sp, #1
    mov     sp, ip
    mrs     r1, cpsr
    and     r1, r1, #0x1f
    teq     r1, #0x17
    bne     Lnot_abt
    bl      func_02059468
    b       Lafter
Lnot_abt:
    teq     r1, #0x1b
    bne     Lafter
    bl      func_02059468
Lafter:
    ldr     ip, =data_020a63a8
    ldr     ip, [ip]
    cmp     ip, #0
Lspin1:
    beq     Lspin1
Lspin2:
    mov     r0, r0
    b       Lspin2
    ldmia   sp!, {r0, r1, r2, r3, ip, lr}
    mov     sp, ip
    bx      lr
}
