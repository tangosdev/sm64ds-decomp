// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02382DC0 size 0x130 (runtime 0x037FAC58, wram)
// C tried by: AR49; asm taken from the AR49 job.
/* storage 0x02382DC0 runtime 0x037FAC58 linked 0x130 wram ARM.
 * MI_CpuCopy8. Unaligned head, then a byte-lane shift copy when the
 * two pointers disagree on alignment, otherwise half/word blocks and
 * an odd tail. No pool. */

asm void fn_02382DC0(void) {
    CMP R2, #0
    BXEQ LR
    TST R1, #1
    BEQ L40
    LDRH IP, [R1, #-1]
    AND IP, IP, #0xFF
    TST R0, #1
    LDRNEH R3, [R0, #-1]
    MOVNE R3, R3, LSR #8
    LDREQH R3, [R0]
    ORR R3, IP, R3, LSL #8
    STRH R3, [R1, #-1]
    ADD R0, R0, #1
    ADD R1, R1, #1
    SUBS R2, R2, #1
    BXEQ LR
L40:
    EOR IP, R1, R0
    TST IP, #1
    BEQ L94
    BIC R0, R0, #1
    LDRH IP, [R0], #2
    MOV R3, IP, LSR #8
    SUBS R2, R2, #2
    BLO L78
L60:
    LDRH IP, [R0], #2
    ORR IP, R3, IP, LSL #8
    STRH IP, [R1], #2
    MOV R3, IP, LSR #16
    SUBS R2, R2, #2
    BHS L60
L78:
    TST R2, #1
    BXEQ LR
    LDRH IP, [R1]
    AND IP, IP, #0xFF00
    ORR IP, IP, R3
    STRH IP, [R1]
    BX LR
L94:
    TST IP, #2
    BEQ LC0
    BICS R3, R2, #1
    BEQ L10C
    SUB R2, R2, R3
    ADD IP, R3, R1
LAC:
    LDRH R3, [R0], #2
    STRH R3, [R1], #2
    CMP R1, IP
    BLO LAC
    B L10C
LC0:
    CMP R2, #2
    BLO L10C
    TST R1, #2
    BEQ LE0
    LDRH R3, [R0], #2
    STRH R3, [R1], #2
    SUBS R2, R2, #2
    BXEQ LR
LE0:
    BICS R3, R2, #3
    BEQ L100
    SUB R2, R2, R3
    ADD IP, R3, R1
LF0:
    LDR R3, [R0], #4
    STR R3, [R1], #4
    CMP R1, IP
    BLO LF0
L100:
    TST R2, #2
    LDRNEH R3, [R0], #2
    STRNEH R3, [R1], #2
L10C:
    TST R2, #1
    BXEQ LR
    LDRH R2, [R1]
    LDRH R0, [R0]
    AND R2, R2, #0xFF00
    AND R0, R0, #0xFF
    ORR R0, R2, R0
    STRH R0, [R1]
    BX LR
}
