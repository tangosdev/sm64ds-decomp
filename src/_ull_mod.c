// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// _ull_mod at 0x01ffa9e8 (ITCM), also known as __aeabi_ulmod -- unsigned 64-bit modulo,
// the runtime helper behind `%` on `unsigned long long`. Arguments arrive in r0:r1
// (numerator) and r2:r3 (denominator); the remainder comes back in r0:r1, outside the C
// ABI for a function returning one value. Toolchain runtime, shipped as assembly, no C
// source for it anywhere in the game.
//
// Shares its body with `_ll_udiv` two rows up: `_ll_udiv` sets r4 = 0 (want quotient) and
// tail-jumps straight past this file's own header into the shared core at `_ull_mod_core`
// (this function's header +0x8); this file sets r4 = 1 (want remainder) and falls
// straight through into the same core. When the denominator's low 32 bits are zero it
// falls out immediately; otherwise, if the DENOMINATOR fits in 32 bits, it calls into
// `_u32_div_f_core` (0x01ffadf8, the second entry point inside `_u32_div_f`, skipping
// that routine's own divide-by-zero check since it has already been ruled out here) --
// and if it does not, it tail-jumps into `_ll_sdiv` (four rows down) at
// `_ll_sdiv_core` (`_ll_sdiv`'s header +0x10, its own "are both operands unsigned-fit"
// check) after packing a "want unsigned remainder" flag into r4, with a companion return
// path at `_ll_sdiv_L074` (`_ll_sdiv`'s own internal label `_L074`, its header +0x74).
// Three live entries into the middle of three different sibling functions -- selected by
// register flags a caller never sees -- is not something any C function signature can
// express. config/arm9/itcm/symbols.txt carries `_ull_mod_core`, `_u32_div_f_core`,
// `_ll_sdiv_core` and `_ll_sdiv_L074` as the label aliases this cross-linkage needs.

extern void _u32_div_f_core(void);
extern void _ll_sdiv_core(void);
extern void _ll_sdiv_L074(void);

asm unsigned long long _ull_mod(unsigned long long num, unsigned long long den)
{
    stmdb sp!, {r4, r5, r6, r7, r11, ip, lr}
    mov r4, #1
    orrs r5, r3, r2
    bne _L018
    ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
    bx lr
_L018:
    orrs r5, r1, r3
    bne _ll_sdiv_L074
    mov r1, r2
    bl _u32_div_f_core
    cmp r4, #0
    movne r0, r1
    mov r1, #0
    ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
    bx lr
    stmdb sp!, {r4, r5, r6, r7, r11, ip, lr}
    mov r4, r1
    orr r4, r4, #1
    b _ll_sdiv_core
}
