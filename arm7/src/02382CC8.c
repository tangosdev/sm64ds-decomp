// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02382CC8 size 0x18 (runtime 0x037FAB60, wram)
// C tried by: the AC C-only redo round; no C attempt reproduced the bytes (see the notes below).
/* storage 0x02382CC8 runtime 0x037FAB60 size 0x18 wram ARM. No pool, no calls.
 * Word copy. Same hand-asm loop shape as fn_02382CB4 (single CMP, single-
 * register LDM/STM pair, R12 end pointer); inline asm for the same reason.
 * (The inline assembler takes no C variable inside { }; R2 is free because
 * n dies in the ADD.) */

int *fn_02382CC8(int *s, int *d, int n) {
    asm {
        ADD R12, d, n
    loop_copy:
        CMP d, R12
        LDMLTIA s!, {R2}
        STMLTIA d!, {R2}
        BLT loop_copy
    }
    return s;
}
