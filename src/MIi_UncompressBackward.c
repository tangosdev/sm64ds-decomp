// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only runtime primitive).
//
// MIi_UncompressBackward at 0x020048d8 -- NitroSDK crt0. Decompresses an LZ-packed image
// in place, back to front: r0 is the end of the packed data, the two words below it give
// the packed size (low 24 bits), the header length (top byte) and how far the output
// grows past r0. A flag byte selects, bit by bit from the top, a literal byte or a
// 12-bit-distance back-reference, and the output range is then cleaned from the data
// cache and invalidated from the instruction cache. _start (src/_start.c) runs it on the
// static image and FS_StartOverlay (src/engine/fs/FS_Overlay.c) on compressed overlays.
//
// The name and the instruction list are pret/pokediamond's crt0.c, where this is an
// `asm` function too. r4-r7 are pushed by hand and the backward `ldmdb` header read has
// no C spelling. Per asm policy.
#ifndef SM64DS_PLATFORM_PC
asm void MIi_UncompressBackward(void *bottom)
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
#else
void MIi_UncompressBackward(void *bottom);
#endif
