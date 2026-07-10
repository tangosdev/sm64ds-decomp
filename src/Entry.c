// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only runtime primitive).
// ARM9 entry point (CRT0): disables IME, switches CPSR modes (SVC/IRQ/SYS) to set up the
// mode stacks in DTCM, runs the autoload/segment initializers, clears DTCM/palette/OAM,
// stores the interrupt-check word, then jumps to the game entry via bx with a hand-loaded
// lr -- no C compiler emits msr/bx-with-pooled-lr, so there is no C to decompile it to.
extern void func_020049f0(void);
extern void func_020048d8(void);
extern void func_0200497c(void);
extern void func_0205a47c(void);
extern void func_01ffafd4(void);
extern void func_02019780(void);
extern void func_02072f94(void);
asm void Entry(void) {
    mov     ip, #0x4000000
    str     ip, [ip, #0x208]
    bl      func_020049f0
    mov     r0, #0x13
    msr     cpsr_c, r0
    ldr     r0, =0x023c0000
    add     r0, r0, #0x3fc0
    mov     sp, r0
    mov     r0, #0x12
    msr     cpsr_c, r0
    ldr     r0, =0x023c0000
    add     r0, r0, #0x3fc0
    sub     r0, r0, #0x40
    mov     sp, r0
    ldr     r1, =0x600
    sub     r1, r0, r1
    mov     r0, #0x1f
    msr     cpsr_fsxc, r0
    sub     sp, r1, #4
    ldr     r1, =0x2004ad8
    ldr     r0, [r1, #0x14]
    bl      func_020048d8
    bl      func_0200497c
    mov     r0, #0
    ldr     r1, =0x023c0000
    mov     r2, #0x4000
    bl      func_0205a47c
    mov     r0, #0
    ldr     r1, =0x5000000
    mov     r2, #0x400
    bl      func_0205a47c
    mov     r0, #0x200
    ldr     r1, =0x7000000
    mov     r2, #0x400
    bl      func_0205a47c
    ldr     r1, =0x023c0000
    add     r1, r1, #0x3fc0
    add     r1, r1, #0x3c
    ldr     r0, =0x1ffd97c
    str     r0, [r1]
    bl      func_01ffafd4
    bl      func_02019780
    bl      func_02072f94
    ldr     r1, =0x2007000
    ldr     lr, =0xffff0000
    bx      r1
}
