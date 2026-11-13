//cpp
// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive:
// IRQ/CPSR control -- FIQ-disable (F bit, 0x40) toggle with dual entry points
// and an indirect literal-pool jump to the shared body). No C to decompile to.
// Per asm policy. Sibling of matched func_0206da64 (CP15 variant).
// The shared body starts at +0x18. mwccarm rejects a local label as an `ldr =`
// operand ("illegal constant expression"), so the literal names it as an offset
// from this function's own symbol; the word resolves to 0x0206d9e4 in the link.

// The C++ entry point IRQ::SetFIQState(bool) is the `ldr`/`bx` at +0x10; the
// link defines it at its ROM address. The block itself keeps C linkage under its
// address name.
extern "C" asm int func_0206d9cc(void) {
    mov r0, #0
    b lbl
    mov r0, #1
    b lbl
lbl:
    ldr r1, =func_0206d9cc+0x18
    bx r1
    mrs r3, cpsr
    mov r1, r3
    and r1, r1, #0x40
    mov r2, #0x40
    tst r0, r0
    orreq r3, r3, r2
    bicne r3, r3, r2
    msr cpsr_fc, r3
    tst r1, r1
    moveq r0, #1
    movne r0, #0
    bx lr
}
