// HAND-ASM PRIMITIVE: byte-faithful asm-block match.
extern void _s32_div_f(void);

asm void func_01ffaa34(void) {
    stmdb   sp!, {r4, r5, r6, r7, r11, r12, lr}
    eor     r4, r1, r3
    mov     r4, r4, asr #1
    mov     r4, r4, lsl #1
    orrs    r5, r3, r2
    bne     _01ffaa54
    ldmia   sp!, {r4, r5, r6, r7, r11, r12, lr}
    bx      lr
_01ffaa54:
    mov     r5, r0, lsr #31
    add     r5, r5, r1
    mov     r6, r2, lsr #31
    add     r6, r6, r3
    orrs    r6, r5, r6
    bne     _01ffaa88
    mov     r1, r2
    bl      _s32_div_f
    ands    r4, r4, #1
    movne   r0, r1
    mov     r1, r0, asr #31
    ldmia   sp!, {r4, r5, r6, r7, r11, r12, lr}
    bx      lr
_01ffaa88:
    cmp     r1, #0
    bge     _01ffaa98
    rsbs    r0, r0, #0
    rsc     r1, r1, #0
_01ffaa98:
    cmp     r3, #0
    bge     _01ffaaa8
    rsbs    r2, r2, #0
    rsc     r3, r3, #0
_01ffaaa8:
    orrs    r5, r1, r0
    beq     _01ffabcc
    mov     r5, #0
    mov     r6, #1
    cmp     r3, #0
    bmi     _01ffaad4
_01ffaac0:
    add     r5, r5, #1
    adds    r2, r2, r2
    adcs    r3, r3, r3
    bpl     _01ffaac0
    add     r6, r6, r5
_01ffaad4:
    cmp     r1, #0
    blt     _01ffaaf4
_01ffaadc:
    cmp     r6, #1
    beq     _01ffaaf4
    sub     r6, r6, #1
    adds    r0, r0, r0
    adcs    r1, r1, r1
    bpl     _01ffaadc
_01ffaaf4:
    mov     r7, #0
    mov     r12, #0
    mov     r11, #0
    b       _01ffab1c
_01ffab04:
    orr     r12, r12, #1
    subs    r6, r6, #1
    beq     _01ffab74
    adds    r0, r0, r0
    adcs    r1, r1, r1
    adcs    r7, r7, r7
_01ffab1c:
    subs    r0, r0, r2
    sbcs    r1, r1, r3
    sbcs    r7, r7, #0
    adds    r12, r12, r12
    adc     r11, r11, r11
    cmp     r7, #0
    bge     _01ffab04
_01ffab38:
    subs    r6, r6, #1
    beq     _01ffab6c
    adds    r0, r0, r0
    adcs    r1, r1, r1
    adc     r7, r7, r7
    adds    r0, r0, r2
    adcs    r1, r1, r3
    adc     r7, r7, #0
    adds    r12, r12, r12
    adc     r11, r11, r11
    cmp     r7, #0
    bge     _01ffab04
    b       _01ffab38
_01ffab6c:
    adds    r0, r0, r2
    adc     r1, r1, r3
_01ffab74:
    ands    r7, r4, #1
    moveq   r0, r12
    moveq   r1, r11
    beq     _01ffabac
    subs    r7, r5, #0x20
    movge   r0, r1, lsr r7
    bge     _01ffabd0
    rsb     r7, r5, #0x20
    mov     r0, r0, lsr r5
    orr     r0, r0, r1, lsl r7
    mov     r1, r1, lsr r5
    b       _01ffabac
_01ffaba4:
    mov     r0, r1, lsr r7
    mov     r1, #0
_01ffabac:
    cmp     r4, #0
    blt     _01ffabbc
    ldmia   sp!, {r4, r5, r6, r7, r11, r12, lr}
    bx      lr
_01ffabbc:
    rsbs    r0, r0, #0
    rsc     r1, r1, #0
    ldmia   sp!, {r4, r5, r6, r7, r11, r12, lr}
    bx      lr
_01ffabcc:
    mov     r0, #0
_01ffabd0:
    mov     r1, #0
    cmp     r4, #0
    blt     _01ffabbc
    ldmia   sp!, {r4, r5, r6, r7, r11, r12, lr}
    bx      lr
}
