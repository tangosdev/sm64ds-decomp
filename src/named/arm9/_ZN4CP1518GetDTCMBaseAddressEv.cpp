//cpp
// @symbol _ZN4CP1518GetDTCMBaseAddressEv
/* CP15::GetDTCMBaseAddress() at 0x020593b8 -- read the DTCM region register and mask off the size field, leaving the
 * base address.
 *
 * HAND-ASM PRIMITIVE: `mrc`/`mcr p15` are coprocessor accesses and no C
 * construct compiles to them, so the asm block is the faithful source rather
 * than a transcription of something lost. The asm-primitive policy is unchanged
 * by this migration; what changes is only that the compiler mangles the symbol
 * instead of the file spelling it by hand. Layout-free (plan phase 1): no
 * `this`, no vtable, no struct, no includers. */
namespace CP15 {

unsigned int GetDTCMBaseAddress(void)
{
    asm { mrc p15,0,r0,c9,c1,0; ldr r1,=0xfffff000; and r0,r0,r1 }
}

}
