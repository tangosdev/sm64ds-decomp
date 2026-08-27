// NONMATCHING (NOT-C-EXPRESSIBLE): byte-exact hand-written asm. A bare epilogue / mid-frame
// exit stub the symbol table split out, not a real function - no standalone C construct
// produces it. Nothing here to match - see notes/arm9-endgame.md.
void func_020731fc(void);
void func_02071ba0(void);

asm void func_020732e8(void) {
    bl func_020731fc
    add r0, r11, #0
    bl func_02071ba0
    add sp, r11, #0x18
    ldmia sp!, {r4, r5, r6, r7, r11, lr}
    bx lr
}
