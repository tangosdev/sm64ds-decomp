// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x0238AE68 size 0x100 (runtime 0x03802D00, wram)
// C tried by: AR57; asm taken from the AR57 job.
/* storage 0x0238AE68 runtime 0x03802D00 linked 0x100 wram ARM.
 * Keeps the redundant halfword narrowings (lsl #16 / lsr #16 / and #1). */

extern void fn_0238AB54(void *p);
extern void fn_0238AB28(void *p);
extern void fn_0238A77C(int a, int b, void *p, int c);
extern void fn_0238A8F8(void *p);
extern void fn_0238A7E0(void *p);

asm void fn_0238AE68(void) {
    STMDB SP!, {R4, R5, LR}
    SUB SP, SP, #0xC
    MOV R4, R0
    AND R5, R4, #1
    MOV R0, R5, LSL #16
    MOV R0, R0, LSR #16
    CMP R0, #1
    BNE done
    ADD R0, SP, #0
    BL fn_0238AB54
    LDRH R1, [SP]
    MOV R0, R1, LSL #30
    MOV R2, R0, LSR #31
    MOV R0, R5, LSL #16
    CMP R2, R0, LSR #16
    BEQ done
    AND R4, R4, #1
    BIC R1, R1, #2
    MOV R0, R4, LSL #16
    MOV R0, R0, LSR #16
    AND R0, R0, #1
    ORR R0, R1, R0, LSL #1
    STRH R0, [SP]
    ADD R0, SP, #0
    BL fn_0238AB28
    MOV R0, #0x86
    MOV R1, #0x10
    ADD R2, SP, #4
    MOV R3, #3
    BL fn_0238A77C
    MOV R0, R4, LSL #16
    MOVS R0, R0, LSR #16
    BNE use_a7e0
    ADD R0, SP, #4
    BL fn_0238A8F8
    B after_a
use_a7e0:
    ADD R0, SP, #4
    BL fn_0238A7E0
after_a:
    MOV R0, #6
    MOV R1, #0x10
    ADD R2, SP, #4
    MOV R3, #3
    BL fn_0238A77C
    MOV R0, #0x86
    MOV R1, #0x50
    ADD R2, SP, #4
    MOV R3, #3
    BL fn_0238A77C
    MOV R0, R5, LSL #16
    MOVS R0, R0, LSR #16
    BNE use_a7e0b
    ADD R0, SP, #4
    BL fn_0238A8F8
    B after_b
use_a7e0b:
    ADD R0, SP, #4
    BL fn_0238A7E0
after_b:
    MOV R0, #6
    MOV R1, #0x50
    ADD R2, SP, #4
    MOV R3, #3
    BL fn_0238A77C
done:
    ADD SP, SP, #0xC
    LDMIA SP!, {R4, R5, LR}
    BX LR
}
