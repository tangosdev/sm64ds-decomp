// HAND-ASM PRIMITIVE: the Nitro SDK's GXi_NopClearFifo128_ (gx_g3x.c, asm in
// pret/pokediamond too): pushes 128 zero words into the FIFO port at r0, four per
// store, through 32 no-writeback stmia.
// It is gx_g3x.c's first function by address and stays its own source.
asm void GXi_NopClearFifo128_(unsigned addr)
{
    mov r1, #0
    mov r2, #0
    mov r3, #0
    mov r12, #0
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    stmia r0, {r1, r2, r3, r12}
    bx lr
}
