// HAND-ASM PRIMITIVE: byte-faithful asm-block match.
extern void _01ffadf8(void);
extern void _01ffaaa8(void);
extern void _01ffaa44(void);

asm void __aeabi_ulmod(void) {
    stmdb   sp!, {r4, r5, r6, r7, r11, ip, lr}
    mov     r4, #1
_01ffa9f0:
    orrs    r5, r3, r2
    bne     _01ffaa00
    ldmia   sp!, {r4, r5, r6, r7, r11, ip, lr}
    bx      lr
_01ffaa00:
    orrs    r5, r1, r3
    bne     _01ffaaa8
    mov     r1, r2
    bl      _01ffadf8
    cmp     r4, #0
    movne   r0, r1
    mov     r1, #0
    ldmia   sp!, {r4, r5, r6, r7, r11, ip, lr}
    bx      lr
    stmdb   sp!, {r4, r5, r6, r7, r11, ip, lr}
    mov     r4, r1
    orr     r4, r4, #1
    b       _01ffaa44
}
