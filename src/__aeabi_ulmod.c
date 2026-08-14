// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// __aeabi_ulmod at 0x01ffa9e8 (ITCM) -- unsigned 64-bit remainder, the runtime helper
// behind `%` on `unsigned long long`. Same multi-entry assembly file as __aeabi_uldiv
// (the 12-byte stub immediately above, r4=0 for quotient) and _ll_sdiv (the signed
// 64-bit body immediately below). There is no original C: a C `%` on unsigned long
// long is a call to this routine, and the window is not one compiled function.
//
// Shared with uldiv: r4 is the want-remainder flag. Divisor 0 returns the dividend.
// Both high halves 0 takes the 32-bit path through .L_01ffadf8 (the uidiv entry that
// skips the divisor-zero guard) and, when r4 is set, moves the remainder from r1 to
// r0 and clears r1. Otherwise it branches into the unsigned 64-bit kernel at
// _ll_sdiv+0x74. The last 16 bytes are the next entry (want-remainder, sign from
// n_hi) that lands at _ll_sdiv+0x10 -- they belong to this sized symbol, not C.
//
// Two results in r0:r1, a mid-body call, two interior branches into the next
// symbol, and a second prologue after bx lr. Same class as _u32_div_f / _ll_sdiv.

asm unsigned long long __aeabi_ulmod(unsigned long long num, unsigned long long den)
{
    stmdb sp!, {r4, r5, r6, r7, r11, r12, lr}
    mov r4, #1
    orrs r5, r3, r2
    bne _nz
    ldmia sp!, {r4, r5, r6, r7, r11, r12, lr}
    bx lr
_nz:
    orrs r5, r1, r3
    dcd 0x1a000027
    mov r1, r2
    dcd 0xeb0000f9
    cmp r4, #0
    movne r0, r1
    mov r1, #0
    ldmia sp!, {r4, r5, r6, r7, r11, r12, lr}
    bx lr
    stmdb sp!, {r4, r5, r6, r7, r11, r12, lr}
    mov r4, r1
    orr r4, r4, #1
    dcd 0xea000003
}
