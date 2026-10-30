// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02381484 size 0x34 (runtime 0x037F931C, wram)
// C tried by: AR47; asm taken from the AR47 branch.
/* storage 0x02381484 runtime 0x037F931C size 0x34 wram. */
/* NitroSDK OS_SaveContext, verbatim. */

asm int fn_02381484(register void *context)
{
    add r1, r0, #0

    mrs r2, cpsr
    str r2, [r1], #0x4

    mov r0, #0xd3
    msr cpsr_c, r0
    str sp, [r1, #0x40]
    msr cpsr_c, r2

    mov r0, #1
    stmia r1, {r0-r14}
    add r0, pc, #8
    str r0, [r1, #0x3c]

    mov r0, #0
    bx lr
}
