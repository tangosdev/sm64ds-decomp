#include "types.h"
/* func_020541b8 at 0x020541b8
 * Takes a pointer to a VRAM bank bits field, clears it, merges bits into
 * the global VRAM register, then calls Vram__Map with those bits.
 * Returns the original bits value.
 */
extern u16 gVramReg;  /* at 0x020a6088 */
extern void Vram__Map(u16 bits);

u16 func_020541b8(u16 *bankBitsPtr)
{
    u16 bits;

    bits = *bankBitsPtr;
    *bankBitsPtr = 0;
    gVramReg = gVramReg | bits;
    Vram__Map(bits);
    return bits;
}
