/* storage 0x023897EC runtime 0x03801684 size 0x24 + 8 pool wram ARM.
 * Two pools (0x0000FFFF constant, in-pack tail-call target). Tail-calls
 * fn_02389818; if the compiler emits BL+epilogue instead of the LDR/BX
 * tail jump this will need whole-asm. */

extern int fn_02389818(int a1);

int fn_023897EC(int a1, int a2, int a3) {
    int v1 = a3 & 0xFFFF;
    int v2 = (a1 & 0x3C00000) << 22;
    int v3 = (a2 & 0x3F0000) << 16;
    return fn_02389818(v1 | (v2 | v3));
}
