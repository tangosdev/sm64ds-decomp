// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x0238B01C size 0x70 (runtime 0x03802EB4, wram)
// C tried by: AR57; asm taken from the AR57 job.
/* storage 0x0238B01C runtime 0x03802EB4 linked 0x70 wram ARM.
 * Shift a byte out on 0x04000138, 9-cycle delays between edges. */

asm void fn_0238B01C(void) {
    MOV IP, #0x4000000
    ADD IP, IP, #0x138
    ORR R1, R0, R1
    LDRH R0, [IP]
    BIC R0, R0, #0x77
    ORR R0, R0, #0x74
    MOV R2, #0
bit:
    BIC R0, R0, #3
    ORR R0, R0, #0
    MOV R3, #1
    TST R3, R1, LSR R2
    MOVNE R3, #1
    MOVEQ R3, #0
    ORR R0, R0, R3
    STRH R0, [IP]
    MOV R3, #9
d1:
    SUBS R3, R3, #1
    BNE d1
    BIC R0, R0, #2
    ORR R0, R0, #2
    STRH R0, [IP]
    MOV R3, #9
d2:
    SUBS R3, R3, #1
    BNE d2
    ADD R2, R2, #1
    CMP R2, #8
    BNE bit
    BX LR
}
