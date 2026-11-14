// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: position-independent boot-ROM thunk (PC-relative).
// @ 0x023805A4 size 0x54 (runtime 0x037F843C, wram)
/* storage 0x023805A4 runtime 0x037F843C size 0x4C + 8 pool wram ARM.
 * IRQ dispatcher: ack the lowest set IRQ bit and BX straight to its handler
 * (table of 25 at 0x03805DF4, stays asm in the gap) with LR preloaded from a
 * pool (0x037F8490, the shared epilogue address). A C switch would make the
 * compiler emit its own table inline, which would land in the wrong place;
 * a C indirect call emits MOV LR,PC instead of the pool LDR LR. So the whole
 * function is asm, like fn_02380594. No relocs (both pools are constants). */

asm void fn_023805A4(void) {
    STMFD SP!, {LR}
    MOV R12, #0x4000000
    ADD R12, R12, #0x210
    LDR R1, [R12, #-8]
    CMP R1, #0
    LDMEQFD SP!, {PC}
    LDMIA R12, {R1, R2}
    ANDS R1, R1, R2
    LDMEQFD SP!, {PC}
    MOV R3, #1
    MOV R0, #0
scan:
    ANDS R2, R1, R3, LSL R0
    ADDEQ R0, R0, #1
    BEQ scan
    STR R2, [R12, #4]
    LDR R1, =0x03805DF4
    LDR R0, [R1, R0, LSL #2]
    LDR LR, =0x037F8490
    BX R0
}
