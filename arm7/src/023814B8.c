// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x023814B8 size 0x2C (runtime 0x037F9350, wram)
// C tried by: AR47; asm taken from the AR47 branch.
/* storage 0x023814B8 runtime 0x037F9350 size 0x2C wram. */
/* NitroSDK OS_LoadContext, verbatim (HW_PSR_CPU_MODE_MASK is 0x1f). */

asm void fn_023814B8(register void *context)
{
    mrs r1, cpsr
    bic r1, r1, #0x1f
    orr r1, r1, #0xd3
    msr cpsr_c, r1

    ldr r1, [r0], #0x4
    msr spsr_fsxc, r1

    ldr sp, [r0, #0x40]

    ldr lr, [r0, #0x3c]
    ldmia r0, {r0-r14}^
    nop

    subs pc, lr, #4
}
