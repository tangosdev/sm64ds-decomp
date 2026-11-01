// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x023977CC size 0x6C (runtime 0x027C9744, main)
// C tried by: AR63; asm taken from the AR63 job.
/* storage 0x023977CC runtime 0x027C9744 linked 0x6C main ARM.
 * The xor tail loads the half back into r0. b56 keeps it in r2. */

asm int fn_023977CC(void) {
    CMP R0, #1
    MOVHI R0, #5
    BXHI LR
    LDR R3, =0x380FFF4
    LDR R2, [R3]
    LDR R1, =0x2FE
    LDRH R1, [R2, R1]!
    BIC R1, R1, #8
    MOV R0, R0, LSL #16
    MOV R0, R0, LSR #16
    AND R0, R0, #1
    ORR R0, R1, R0, LSL #3
    STRH R0, [R2]
    LDR R0, [R3]
    ADD R0, R0, #0x200
    LDRH R0, [R0, #0xFE]
    MOV R1, R0, LSL #26
    MOV R1, R1, LSR #31
    MOV R0, R0, LSL #28
    EOR R1, R1, R0, LSR #31
    LDR R0, =0x4808290
    STRH R1, [R0]
    MOV R0, #0
    BX LR
}
