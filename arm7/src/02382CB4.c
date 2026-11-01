// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02382CB4 size 0x14 (runtime 0x037FAB4C, wram)
// C tried by: the AC C-only redo round; no C attempt reproduced the bytes (see the notes below).
/* storage 0x02382CB4 runtime 0x037FAB4C size 0x14 wram ARM. No pool, no calls.
 * Word fill. The single-CMP test with single-register STM and the R12 end
 * pointer is not producible from a C loop in 2004/b56 or 1.2 (both emit STR
 * with a duplicate CMP, or a prologue branch), so the loop is inline asm.
 * (The inline assembler takes no C variable inside { }; v arrives in R0.) */

int fn_02382CB4(int v, int *p, int n) {
    asm {
        ADD R12, p, n
    loop_fill:
        CMP p, R12
        STMLTIA p!, {R0}
        BLT loop_fill
    }
    return v;
}
