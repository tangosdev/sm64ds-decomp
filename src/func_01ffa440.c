// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ffa440 at 0x01ffa440 (ITCM) -- unsigned 32-bit integer to float conversion. The
// compiler emits a call to this for `(float)` on an `unsigned int`; toolchain runtime,
// shipped as assembly, no C source for it anywhere in the game. It sits directly between
// func_01ffa344 (float-to-int, its inverse) and func_01ffa4bc (the SIGNED int-to-float
// conversion, "the inverse of func_01ffa344 ... part of the same soft-float runtime
// block") -- three conversions in a row, one hand-written block.
//
// Unlike func_01ffa4bc it takes no sign off the top: an already-negative (as signed)
// value is, reinterpreted unsigned, already >= 2**31 and needs no `clz` normalisation at
// all, so `bmi _L01c` skips straight past the leading-zero count and shift. That one
// `bmi` is the entire difference from the signed routine's shape.
//
// Its own inexact/rounding tail is short (an early `bxne lr` where func_01ffa4bc reads
// the FP status word again) because it reuses TWO addresses inside func_01ffa4bc's body
// directly: `_i2f_addone` (0x01ffa538, func_01ffa4bc's own header +0x7c, a "round up by
// one and return" fragment with no label of its own there) and `_i2f_trap308`
// (0x01ffa558, func_01ffa4bc's header +0x9c, the inexact-trap tail). Both addresses are
// unreachable by fall-through from anywhere inside func_01ffa4bc.c itself -- they are
// reached only by this function's external jump into the middle of a sibling's body,
// which no C construct can express. config/arm9/itcm/symbols.txt carries `_i2f_addone`
// and `_i2f_trap308` as the label aliases this cross-linkage needs.

extern int func_0207322c(void);
extern void _i2f_addone(void);
extern void _i2f_trap308(void);

asm float func_01ffa440(unsigned int value)
{
    cmp r0, #0
    bxeq lr
    mov r3, #0x9e
    bmi _L01c
    clz ip, r0
    movs r0, r0, lsl ip
    sub r3, r3, ip
_L01c:
    ands r2, r0, #0xff
    add r0, r0, r0
    mov r0, r0, lsr #9
    orr r0, r0, r3, lsl #23
    bxeq lr
    stmdb sp!, {r0, r2, r3, ip, lr}
    bl func_0207322c
    mov r1, r0
    ldmia sp!, {r0, r2, r3, ip, lr}
    ldr r3, [r1]
    tst r3, #0x1000
    bne _i2f_trap308
    orr r3, r3, #0x10
    str r3, [r1]
    tst r3, #0x800000
    bxne lr
    tst r3, #0x400000
    bne _i2f_addone
    tst r2, #0x80
    bxeq lr
    ands r1, r2, #0x7f
    andeqs r1, r0, #1
    addne r0, r0, #1
    bx lr
}
