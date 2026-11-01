// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x0238E0E4 size 0x3C (runtime 0x027C005C, main)
// C tried by: AR59; asm taken from the AR59 job.
/* storage 0x0238E0E4 runtime 0x027C005C linked 0x3C main ARM.
 * Sum 16 bytes at dat_27D6A84+0x1614 and tail-call fn_0238E0BC(sum>>4).
 * The opening mov r2,#0 / mov r3,r2 is the fresh-scratch choice b56
 * inverts; kept here as written. */

extern unsigned char dat_27D6A84[];
extern int fn_0238E0BC(unsigned int a1);

asm int fn_0238E0E4(void) {
    MOV R2, #0
    MOV R3, R2
    LDR R1, =dat_27D6A84
loop:
    ADD R0, R1, R3
    ADD R0, R0, #0x1000
    LDRB R0, [R0, #0x614]
    ADD R2, R2, R0
    ADD R3, R3, #1
    CMP R3, #0x10
    BLT loop
    MOV R0, R2, LSR #4
    LDR IP, =fn_0238E0BC
    BX IP
}
