// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02386068 size 0x98 (runtime 0x037FDF00, wram)
// C tried by: AH16; asm taken from the AH16 branch.
/* storage 0x02386068 runtime 0x037FDF00 size 0x98 wram ARM.
 * For each live track in unk_3807B04, if a channel's word at +0x28
 * sits in [a1, a2], clear the track. Whole-asm: the track pointer and
 * the channel stay in r2 and r3 across the leaf, and a separate TU
 * spills every value live across an extern BL. */

extern unsigned char unk_3807B04[];
extern unsigned char *fn_023859CC(int a1, int a2);
extern int fn_0238592C(unsigned char *a1);

asm void fn_02386068(unsigned int a1, unsigned int a2) {
    stmfd sp!, {r4, r5, r6, r7, r8, r9, lr}
    sub sp, sp, #4
    mov r8, r0
    mov r7, r1
    mov r6, #0
    mov r4, r6
    ldr r5, =unk_3807B04
    mov r9, #0x24
outer:
    mul r0, r6, r9
    add r2, r5, r0
    ldrb r0, [r5, r0]
    cmp r0, #0
    beq next
    mov r3, r4
    b ichk
inner:
    mov r0, r2
    mov r1, r3
    bl fn_023859CC
    cmp r0, #0
    beq inc
    ldr r0, [r0, #0x28]
    cmp r8, r0
    bhi inc
    cmp r0, r7
    bhi inc
    mov r0, r2
    bl fn_0238592C
    b next
inc:
    add r3, r3, #1
ichk:
    cmp r3, #0x10
    blt inner
next:
    add r6, r6, #1
    cmp r6, #0x10
    blt outer
    add sp, sp, #4
    ldmfd sp!, {r4, r5, r6, r7, r8, r9, lr}
    bx lr
}
