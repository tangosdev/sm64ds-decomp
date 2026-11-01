// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: interworking veneer: tail call through a pooled pointer with BX, no C body (a C wrapper adds a frame or BL).
// @ 0x0238C098 size 0x10 (runtime 0x03803F30, wram)
/* storage 0x0238C098 runtime 0x03803F30 size 0x10 wram ARM.
 * Call pair head: pushes a word of stack and runs the two init hooks.
 * Falls through into fn_0238C0A8 (the shared epilogue) — no return here.
 * Whole-asm: a C function would emit its own epilogue and overshoot 0x10. */

extern void fn_023833CC(void); /* =0x037FB264 */
extern void fn_02382B38(void); /* =0x037FA9D0 */

asm void fn_0238C098(void) {
    STMDB SP!, {LR}
    SUB SP, SP, #4
    BL fn_023833CC
    BL fn_02382B38
}
