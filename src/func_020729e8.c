// NONMATCHING (NOT-C-EXPRESSIBLE): byte-exact hand-written asm. A bare epilogue / mid-frame
// exit stub the symbol table split out, not a real function - no standalone C construct
// produces it. Nothing here to match - see notes/arm9-endgame.md.
asm void func_020729e8(void) { add sp, sp, #0xac; ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}; bx lr }
