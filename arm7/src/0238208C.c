// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x0238208C size 0x10 (runtime 0x037F9F24, wram)
// C tried by: the AC C-only redo round; no C attempt reproduced the bytes (see the notes below).
/* storage 0x0238208C runtime 0x037F9F24 size 0x10 wram ARM. Tail wrapper over fn_0238209C.
 * Whole-asm: every C tail spelling sibcalls at b56 -O4,s (LDR R12,=fn; BX R12, 0xC).
 * Pre-UAL mnemonics (this assembler predates PUSH/POP); same words as the ROM. */

extern int fn_0238209C(void);

asm void fn_0238208C(void) {
    stmfd sp!, {lr}
    bl fn_0238209C
    ldmfd sp!, {lr}
    bx lr
}
