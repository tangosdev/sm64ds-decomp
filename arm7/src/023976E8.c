// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x023976E8 size 0xE4 (runtime 0x027C9660, main)
// C tried by: AR63; asm taken from the AR63 job.
/* storage 0x023976E8 runtime 0x027C9660 linked 0xE4 main ARM.
 * Same xor tail as 023977CC: the half comes back in r0. */

asm int fn_023976E8(void) {
    CMP R0, #1
    BHI bad
    CMP R1, #1
    BLS ok
bad:
    MOV R0, #5
    BX LR
ok:
    CMP R0, #0
    BEQ arm0
    CMP R0, #1
    BEQ arm1
    B join
arm0:
    LDR R2, =0x380FFF4
    LDR R3, [R2]
    LDR R2, =0x2FE
    LDRH R2, [R3, R2]!
    BIC R2, R2, #0x20
    MOV R1, R1, LSL #16
    MOV R1, R1, LSR #16
    AND R1, R1, #1
    ORR R1, R2, R1, LSL #5
    STRH R1, [R3]
    B join
arm1:
    LDR R1, =0x380FFF4
    LDR R2, [R1]
    ADD R1, R2, #0x200
    LDRH R1, [R1, #0xF2]
    CMP R1, #1
    MOVNE R0, #0xB
    BXNE LR
    LDR R1, =0x2FE
    LDRH R1, [R2, R1]!
    BIC R1, R1, #0x20
    STRH R1, [R2]
join:
    LDR R3, =0x380FFF4
    LDR R2, [R3]
    LDR R1, =0x2FE
    LDRH R1, [R2, R1]!
    BIC R1, R1, #0x10
    MOV R0, R0, LSL #16
    MOV R0, R0, LSR #16
    AND R0, R0, #1
    ORR R0, R1, R0, LSL #4
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
