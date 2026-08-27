// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only runtime primitive).
// LZ-style backward decompressor: reads the packed size/header via ldmdb, walks the
// buffer backwards emitting literals/back-references by a rolling bit flag, then cleans
// the data cache (mcr p15). The backward ldmdb walk and the cache maintenance are not
// expressible in C under our flags. Per asm policy.
asm void func_020048d8(void)
{
    cmp     r0, #0
    beq     L_end
    stmdb   sp!, {r4, r5, r6, r7}
    ldmdb   r0, {r1, r2}
    add     r2, r0, r2
    sub     r3, r0, r1, lsr #24
    bic     r1, r1, #0xff000000
    sub     r1, r0, r1
    mov     r4, r2
L_outer:
    cmp     r3, r1
    ble     L_cache_setup
    ldrb    r5, [r3, #-1]!
    mov     r6, #8
L_bit:
    subs    r6, r6, #1
    blt     L_outer
    tst     r5, #0x80
    bne     L_copy
    ldrb    r0, [r3, #-1]!
    strb    r0, [r2, #-1]!
    b       L_next
L_copy:
    ldrb    ip, [r3, #-1]!
    ldrb    r7, [r3, #-1]!
    orr     r7, r7, ip, lsl #8
    bic     r7, r7, #0xf000
    add     r7, r7, #2
    add     ip, ip, #0x20
L_inner:
    ldrb    r0, [r2, r7]
    strb    r0, [r2, #-1]!
    subs    ip, ip, #0x10
    bge     L_inner
L_next:
    cmp     r3, r1
    mov     r5, r5, lsl #1
    bgt     L_bit
L_cache_setup:
    bic     r3, r1, #0x1f
L_cache:
    mcr     p15, 0, r3, c7, c5, 1
    mcr     p15, 0, r3, c7, c14, 1
    add     r3, r3, #0x20
    cmp     r3, r4
    blt     L_cache
    ldmia   sp!, {r4, r5, r6, r7}
L_end:
    bx      lr
}
