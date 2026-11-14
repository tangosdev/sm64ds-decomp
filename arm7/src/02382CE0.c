// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02382CE0 size 0x4C (runtime 0x037FAB78, wram)
// C tried by: AR49; asm taken from the AR49 job.
/* storage 0x02382CE0 runtime 0x037FAB78 linked 0x4C wram ARM.
 * MI_CpuFill32. Hand schedule: eight copies of the fill word, then a
 * predicated STMIA writeback of {r0,r2-r8} and a one-word tail.
 * No pool. */

asm void fn_02382CE0(void) {
    STMDB SP!, {R4-R9}
    ADD R9, R1, R2
    MOV IP, R2, LSR #5
    ADD IP, R1, IP, LSL #5
    MOV R2, R0
    MOV R3, R2
    MOV R4, R2
    MOV R5, R2
    MOV R6, R2
    MOV R7, R2
    MOV R8, R2
word:
    CMP R1, IP
    STMLTIA R1!, {R0, R2-R8}
    BLT word
tail:
    CMP R1, R9
    STMLTIA R1!, {R0}
    BLT tail
    LDMIA SP!, {R4-R9}
    BX LR
}
