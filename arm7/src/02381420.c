// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02381420 size 0x64 (runtime 0x037F92B8, wram)
// C tried by: AR47; asm taken from the AR47 branch.
/* storage 0x02381420 runtime 0x037F92B8 size 0x64 wram. */
/* NitroSDK OS_InitContext, predating the tst/subne stack-alignment fix. */

asm void fn_02381420(register void *context, register unsigned int newpc, register unsigned int newsp)
{
    add newpc, newpc, #4
    str newpc, [context, #0x40]

    str newsp, [context, #0x44]
    sub newsp, newsp, #0x40
    str newsp, [context, #0x38]

    ands r1, newpc, #1
    movne r1, #0x3f
    moveq r1, #0x1f
    str r1, [context]

    mov r1, #0
    str r1, [context, #0x4]
    str r1, [context, #0x8]
    str r1, [context, #0xc]
    str r1, [context, #0x10]
    str r1, [context, #0x14]
    str r1, [context, #0x18]
    str r1, [context, #0x1c]
    str r1, [context, #0x20]
    str r1, [context, #0x24]
    str r1, [context, #0x28]
    str r1, [context, #0x2c]
    str r1, [context, #0x30]
    str r1, [context, #0x34]
    str r1, [context, #0x3c]

    bx lr
}
