// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// _ll_udiv at 0x01ffa9dc (ITCM), also known as __aeabi_uldiv -- unsigned 64-bit divide,
// the runtime helper behind `/` on `unsigned long long`. Toolchain runtime, shipped as
// assembly, no C source for it anywhere in the game.
//
// The whole body is a two-instruction header: push the frame, flag r4 = 0 (the shared
// core's "want quotient" selector), then tail-jump straight into `_ull_mod`'s body at its
// shared entry, `_ull_mod_core` (0x01ffa9f0, `_ull_mod`'s header +0x8) -- skipping
// `_ull_mod`'s own header, which sets the same r4 flag to 1 instead. A live entry into
// the middle of a sibling function, selected purely by which value a caller-invisible
// register holds, is not something a C function signature can express.
// config/arm9/itcm/symbols.txt carries `_ull_mod_core` as the label alias this needs.

extern void _ull_mod_core(void);

asm unsigned long long _ll_udiv(unsigned long long num, unsigned long long den)
{
    stmdb sp!, {r4, r5, r6, r7, r11, ip, lr}
    mov r4, #0
    b _ull_mod_core
}
