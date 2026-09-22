// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ffa440 at 0x01ffa440 (ITCM) -- unsigned 32-bit integer to float conversion, the
// unsigned twin of func_01ffa4bc immediately below it and part of the same soft-float
// runtime block. Assembly in the original.
//
// It skips the sign work func_01ffa4bc opens with. Zero returns straight away, and the
// `bmi` skips the `clz` / `lsls` normalisation whenever bit 31 is already set, because a
// value with the top bit set is already in range at the 0x9e exponent bias (127 + 31).
// From there it runs the same tail its signed twin does: the eight bits shifted out of
// the bottom are captured in r2, and if any of them are set the result is inexact, so it
// reads the FP status word through func_0207322c, sets the inexact flag, and then picks a
// rounding direction from the mode bits -- round-to-nearest with ties to even (the
// `ands r1, r2, #0x7f` / `andeqs r1, r0, #1` pair), toward +infinity, or toward zero.
//
// Two of the branches leave the function body. The `bne` at +0x48 goes to 0x01ffa558 and
// the `bne` at +0x60 goes to 0x01ffa538, both of which sit inside func_01ffa4bc's address
// range on instructions that are unreachable from func_01ffa4bc's own entry: the
// assembler laid this routine's two out-of-line tails out inside its neighbour.
// 0x01ffa538 is the `add r0, r0, #1; bx lr` round-away-from-zero tail, and 0x01ffa558 is
// the enabled-trap exit that leaves through func_0206ddcc with the packed exception code
// 0x40000308. No C construct expresses a jump into another function's interior, so both
// are named by their addresses and branched to as externs -- the same idea `_fadd_core`
// and `_fsub_core` use to tie func_01ff9378 and func_01ffa594 together four rows down,
// but without a config/arm9/itcm/symbols.txt row: those two labels sit in ranges no
// src/ file is enrolled for, so a delinked gap object defines them, while 0x01ffa538 and
// 0x01ffa558 sit inside src/func_01ffa4bc.c's enrolled range, where nothing would. A row
// there is reported by `dsd check symbols` as a symbol missing from the linked binary.
//
// For the same reason this file is deliberately not added to config/arm9/itcm/delinks.txt:
// mwldarm cannot resolve either branch, so enrolling it fails the link outright. That
// matches how src/func_01ff9378.c and src/func_01ffa594.c are already carried -- byte-
// matched sources that the ROM build takes from the gap object.
//
// The same two names are also why `tools/eligible.py` classifies this file
// "unresolvable": its rule 5 requires every undefined reference to name a symbol that
// config/**/symbols.txt defines, and neither address has a row, for the reason above.
// What that rule is guarding against does not apply here -- `tools/linkcheck.py --name
// func_01ffa440` resolves both branches by address and reports VERIFIED with 0 blind
// words, so every byte of both targets is checked against the ROM. They are simply
// unnamed, which is why the file records the routine rather than building it.
//
// The argument registers are pushed across the call as they are in every other routine in
// this block, which is the tell that these are one hand-written assembly file.

extern int func_0207322c(void);
extern void func_01ffa538(void);
extern void func_01ffa558(void);

asm float func_01ffa440(unsigned int value)
{
    cmp r0, #0
    bxeq lr
    mov r3, #0x9e
    bmi _L01c
    clz ip, r0
    movs r0, r0, lsl ip
    sub r3, r3, ip
_L01c:
    ands r2, r0, #0xff
    add r0, r0, r0
    mov r0, r0, lsr #9
    orr r0, r0, r3, lsl #23
    bxeq lr
    stmdb sp!, {r0, r2, r3, ip, lr}
    bl func_0207322c
    mov r1, r0
    ldmia sp!, {r0, r2, r3, ip, lr}
    ldr r3, [r1]
    tst r3, #0x1000
    bne func_01ffa558
    orr r3, r3, #0x10
    str r3, [r1]
    tst r3, #0x800000
    bxne lr
    tst r3, #0x400000
    bne func_01ffa538
    tst r2, #0x80
    bxeq lr
    ands r1, r2, #0x7f
    andeqs r1, r0, #1
    addne r0, r0, #1
    bx lr
}
