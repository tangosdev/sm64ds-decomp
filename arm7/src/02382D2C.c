// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02382D2C size 0x94 (runtime 0x037FABC4, wram)
// C tried by: AR49; asm taken from the AR49 job.
/* storage 0x02382D2C runtime 0x037FABC4 linked 0x94 wram ARM.
 * MI_CpuFill8. Odd-byte head through a halfword RMW, then half, then
 * words, then a predicated half and an odd tail. No pool. */

asm void fn_02382D2C(void) {
    CMP R2, #0
    BXEQ LR
    TST R0, #1
    BEQ half_ready
    LDRH IP, [R0, #-1]
    AND IP, IP, #0xFF
    ORR R3, IP, R1, LSL #8
    STRH R3, [R0, #-1]
    ADD R0, R0, #1
    SUBS R2, R2, #1
    BXEQ LR
half_ready:
    CMP R2, #2
    BLO byte_tail
    ORR R1, R1, R1, LSL #8
    TST R0, #2
    BEQ word_ready
    STRH R1, [R0], #2
    SUBS R2, R2, #2
    BXEQ LR
word_ready:
    ORR R1, R1, R1, LSL #16
    BICS R3, R2, #3
    BEQ half_tail
    SUB R2, R2, R3
    ADD IP, R3, R0
word:
    STR R1, [R0], #4
    CMP R0, IP
    BLO word
half_tail:
    TST R2, #2
    STRNEH R1, [R0], #2
byte_tail:
    TST R2, #1
    BXEQ LR
    LDRH R3, [R0]
    AND R3, R3, #0xFF00
    AND R1, R1, #0xFF
    ORR R1, R1, R3
    STRH R1, [R0]
    BX LR
}
