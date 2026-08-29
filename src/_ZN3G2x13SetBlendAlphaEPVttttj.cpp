//cpp
// @symbol _ZN3G2x13SetBlendAlphaEPVttttj
#include "G2x.h"

/* The final argument is 32-bit: the ROM loads its stack slot with `ldr`, not
   `ldrh`. The former `...ttttt` name contradicted that instruction. */
void G2x::SetBlendAlpha(volatile u16 *reg, u16 firstTarget,
                        u16 secondTarget, u16 firstAlpha,
                        u32 secondAlpha)
{
    *(volatile u32 *)reg =
        ((firstTarget | 0x40) | (secondTarget << 8)) |
        ((firstAlpha | (secondAlpha << 8)) << 16);
}
