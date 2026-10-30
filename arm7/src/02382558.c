// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02382558 size 0x10 (runtime 0x037FA3F0, wram)
// C tried by: AR4, AR48; asm taken from the AR48 branch.
/* storage 0x02382558 runtime 0x037FA3F0 size 0x10 wram ARM. Tail wrapper over fn_023825E4.
 * Whole-asm like AR4's fn_0238208C: every C tail spelling sibcalls at b56 -O4,s. */

extern int fn_023825E4(void);

asm void fn_02382558(void) {
    stmfd sp!, {lr}
    bl fn_023825E4
    ldmfd sp!, {lr}
    bx lr
}
