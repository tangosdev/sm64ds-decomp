// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: ARM7 crt0 (startup: stack/mode setup via MSR, autoload copy, BSS clear).
// @ 0x02380000 size 0x168 (crt0)
.section .text
.arm
.global _start
_start:
	mov ip, #0x4000000
	str ip, [ip, #0x208]
	ldr r1, [pc, #0x6c]
	mov r0, #0x3800000
	cmp r0, r1
	movpl r1, r0
	ldr r2, [pc, #0x60]
	mov r0, #0
L_02380020:
	cmp r1, r2
	stmltia r1!, {r0}
	blt L_02380020
	mov r0, #0x13
	msr cpsr_c, r0
	ldr sp, [pc, #0x48]
	mov r0, #0x12
	msr cpsr_c, r0
	ldr r0, [pc, #0x40]
	mov sp, r0
	ldr r1, [pc, #0x3c]
	sub r1, r0, r1
	mov r0, #0x1f
	msr cpsr_fsxc, r0
	sub sp, r1, #4
	bl L_023800A0
	bl L_02380114
	ldr r1, [pc, #0x24]
	ldr r0, [pc, #0x24]
	str r0, [r1]
	ldr r1, [pc, #0x20]
	ldr lr, [pc, #0x20]
	bx r1
	.word 0x02380168
	.word 0x0380FF00
	.word 0x0380FFC0
	.word 0x0380FF80
	.word 0x00000200
	.word 0x0380FFFC
	.word 0x037F843C
	.word 0x037F8300
	.word 0xFFFF0000
fn_023800A0:
L_023800A0:
	ldr r0, [pc, #0x64]
	ldr r1, [r0]
	ldr r2, [r0, #4]
	ldr r3, [r0, #8]
L_023800B0:
	cmp r1, r2
	beq L_023800F0
	ldr r4, [r1], #4
	ldr r5, [r1], #4
	add r6, r4, r5
L_023800C4:
	cmp r4, r6
	ldrmi r7, [r3], #4
	strmi r7, [r4], #4
	bmi L_023800C4
	ldr r5, [r1], #4
	add r6, r4, r5
	mov r7, #0
L_023800E0:
	cmp r4, r6
	strlo r7, [r4], #4
	blo L_023800E0
	beq L_023800B0
L_023800F0:
	ldr r4, [r0, #0xc]
	ldr r6, [r0, #0x10]
	mov r7, #0
L_023800FC:
	cmp r4, r6
	strlo r7, [r4], #4
	blo L_023800FC
	b L_02380110
	.word 0x02380154
L_02380110:
	bx lr
fn_02380114:
L_02380114:
	mov r0, #1
	mov r1, #0
	ldr r2, [pc, #0x28]
	sub r3, r2, #0x400000
L_02380124:
	strh r1, [r2]
	ldrh ip, [r3]
	cmp r1, ip
	movne r0, #2
	bne L_02380144
	add r1, r1, #1
	cmp r1, #2
	bne L_02380124
L_02380144:
	strh r0, [r2]
	bx lr
	.word 0x027FFFFA
	.word 0x00000000
	.word 0x023A4B0C
	.word 0x023A4B24
	.word 0x02380168
	.word 0x02380168
	.word 0x02380168
