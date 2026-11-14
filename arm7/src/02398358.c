// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02398358 size 0x1A8 (runtime 0x027CA2D0, main)
// C tried by: AR44; asm taken from the AR44 branch.
/* storage 0x02398358 runtime 0x027CA2D0 size 0x190 + pool 0x1A8 main ARM.
 * Sound-channel register self-test over 27 channel regs: masked
 * write/readback, 0x1234 ramp write/readback, zero write/readback, then
 * sets a status bit when any readback mismatched.
 * Whole-asm enrollment as an `asm` function (not inline `asm {}`): the
 * zero-loop's store writeback (`strh [r1,r0]!`) is unreachable from C
 * (~30 probes: every C store+reload recomputes the table offset), and
 * the prologue's `mov r1,r2` / `mov r0,r2` pair cannot survive inline
 * asm under b56 -O4,s (copy coalescing rewrites the second source to
 * r1; immune to opt_propagation/common_subs/lifetimes/dead_code/
 * strength pragmas, block splits, and lone labels). The `asm` function
 * body assembles verbatim, so the pair survives; pools emit in
 * first-use order at function end. */
asm void fn_02398358(void) {
    stmfd sp!, {r4-r8, r14}
    mov r2, #0
    mov r1, r2
    mov r0, r2
    ldr r7, =0x027D67A0
    ldr r5, =0x027D672C
    ldr r12, =0x04808000
    b L74
L20:
    mov r3, r0
    mov r6, r1, lsl #1
    b L68
L2C:
    mov r4, r3, lsl #2
    ldrh r4, [r7, r4]
    add r8, r4, r12
    ldrh r4, [r5, r6]
    add r14, r7, r3, lsl #2
    ldrh r14, [r14, #2]
    and r4, r4, r14
    strh r4, [r8]
    ldrh r14, [r8]
    cmp r14, r4
    beq L64
    cmp r2, #0x20
    add r2, r2, #1
    bhi L168
L64:
    add r3, r3, #1
L68:
    cmp r3, #0x1b
    bcc L2C
    add r1, r1, #1
L74:
    cmp r1, #3
    bcc L20
    mov r4, #0
    ldr r5, =0x1234
    ldr r3, =0x027D67A0
    mov r1, r5
L8C:
    mov r12, r4, lsl #2
    add r0, r3, r4, lsl #2
    ldrh r0, [r0, #2]
    and r14, r5, r0
    ldrh r0, [r3, r12]
    add r0, r0, #0x4800000
    add r0, r0, #0x8000
    strh r14, [r0]
    add r0, r5, r1
    mov r0, r0, lsl #16
    mov r5, r0, lsr #16
    add r4, r4, #1
    cmp r4, #0x1b
    bcc L8C
    mov r0, #0
    ldr r4, =0x1234
    ldr r14, =0x027D67A0
    mov r1, r4
    b LD4
LD8:
    mov r3, r0, lsl #2
    ldrh r3, [r14, r3]
    add r3, r3, #0x4800000
    add r3, r3, #0x8000
    ldrh r12, [r3]
    add r3, r14, r0, lsl #2
    ldrh r3, [r3, #2]
    and r3, r4, r3
    cmp r12, r3
    beq L10C
    cmp r2, #0x20
    add r2, r2, #1
    bhi L168
L10C:
    add r3, r4, r1
    mov r3, r3, lsl #16
    mov r4, r3, lsr #16
    add r0, r0, #1
LD4:
    cmp r0, #0x1b
    bcc LD8
    mov r4, #0
    ldr r12, =0x027D67A0
    mov r3, r4
    ldr r0, =0x04808000
    b L160
L138:
    mov r1, r4, lsl #2
    ldrh r1, [r12, r1]
    strh r3, [r1, r0]!
    ldrh r1, [r1]
    cmp r1, #0
    beq L15C
    cmp r2, #0x20
    add r2, r2, #1
    bhi L168
L15C:
    add r4, r4, #1
L160:
    cmp r4, #0x1b
    bcc L138
L168:
    cmp r2, #0
    beq L188
    ldr r0, =0x0380FFF4
    ldr r1, [r0]
    ldr r0, =0x302
    ldrh r0, [r1, r0]!
    orr r0, r0, #1
    strh r0, [r1]
L188:
    ldmfd sp!, {r4-r8, r14}
    bx r14
}
