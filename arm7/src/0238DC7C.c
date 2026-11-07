// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: context restore (IME/IRQ return path).
// @ 0x0238DC7C size 0x70 (runtime 0x03805B14, wram)
/* Hand-asm: context restore, not a C body. Enables interrupts via IME
   (0x04000208), clears the flag word at 0x0380FFFC, posts 0x100 to the
   0x04000180 register and spins on its low nibble, then reloads r0-r10 from
   the block at 0x04000000 and jumps to the saved pc in *0x027FFE34.
   Pool: 0x0380FFFC, 0x04000180, 0x027FFE00. */

asm void fn_0238DC7C(void)
{
    mov r12, #0x4000000
    str r12, [r12, #0x208]
    ldr r1, =0x0380FFFC
    mov r0, #0
    str r0, [r1]
    ldr r1, =0x04000180
    mov r0, #0x100
    strh r0, [r1]
loc_238DC9C:
    ldrh r0, [r1]
    and r0, r0, #0xF
    cmp r0, #1
    bne loc_238DC9C
    ldr r1, =0x04000180
    mov r0, #0
    strh r0, [r1]
loc_238DCB8:
    ldrh r0, [r1]
    cmp r0, #1
    beq loc_238DCB8
    ldr r3, =0x027FFE00
    ldr r12, [r3, #0x34]
    mov lr, r12
    mov r11, #0x4000000
    ldmdb r11, {r0-r10}
    mov r11, #0
    bx r12
}
