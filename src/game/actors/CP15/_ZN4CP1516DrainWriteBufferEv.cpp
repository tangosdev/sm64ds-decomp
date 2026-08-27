//cpp
// @symbol _ZN4CP1516DrainWriteBufferEv
/* CP15::DrainWriteBuffer() at 0x02058c5c -- block until the write buffer has
 * emptied, so a following read sees what was just written.
 *
 * HAND-ASM PRIMITIVE. `mcr p15' is a coprocessor write; no C construct compiles
 * to it, so the asm block is the faithful source rather than a transcription of
 * something lost. That policy is unchanged by this migration -- what changes is
 * only that the symbol is mangled by the compiler instead of spelled by hand.
 *
 * PROBE: mwccarm's whole-function `asm void' form inside a namespace. The
 * function has no `this', no vtable and no struct, so this is the plan's
 * layout-free case (phase 1) and nothing else in the tree can be affected. */
namespace CP15 {

asm void DrainWriteBuffer(void)
{
    mov r0, #0
    mcr p15, 0, r0, c7, c10, 4
    bx lr
}

}
