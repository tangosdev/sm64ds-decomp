// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: Thumb SWI veneers (SWI calls into the ARM7 BIOS).
// @ 0x02388130 size 0x7C (wram)
.section .text
.thumb
fn_02388130:
	swi 0
	bx lr
fn_02388134:
	swi 3
	bx lr
fn_02388138:
	ldr r2, [pc, #8]
	mov ip, r2
	mov r2, #0
	swi 4
	bx lr
	lsl r0, r0, #0
	.word 0x04000000
fn_02388148:
	mov r2, #0
	swi 5
	bx lr
fn_0238814E:
	swi 6
	bx lr
fn_02388152:
	swi 7
	bx lr
fn_02388156:
	swi 8
	bx lr
fn_0238815A:
	add r1, r0, #0
	mov r0, #1
	swi 8
	bx lr
fn_02388162:
	add r1, r0, #0
	mov r0, #0
	swi 8
	bx lr
fn_0238816A:
	swi 9
	bx lr
	swi 9
	add r0, r1, #0
	bx lr
fn_02388174:
	swi 0xb
	bx lr
fn_02388178:
	swi 0xc
	bx lr
fn_0238817C:
	swi 0xd
	bx lr
fn_02388180:
	swi 0xe
	bx lr
fn_02388184:
	swi 0xf
	bx lr
fn_02388188:
	swi 0x10
	bx lr
fn_0238818C:
	swi 0x11
	bx lr
fn_02388190:
	swi 0x12
	bx lr
fn_02388194:
	swi 0x13
	bx lr
fn_02388198:
	swi 0x14
	bx lr
fn_0238819C:
	swi 0x15
	bx lr
fn_023881A0:
	swi 0x1a
	bx lr
fn_023881A4:
	swi 0x1b
	bx lr
fn_023881A8:
	swi 0x1c
	bx lr
