#include "types.h"
/* Event::SetBit(u32 bit) at 0x02029ec4
 * Free function in namespace Event (no `this`). Sets bit `bit` in the global
 * event bitfield data_0209f34c. Declared in SM64DS_2.h:
 *   namespace Event { void SetBit(u32 bit); ... }
 *
 * Reloc resolves to data_0209f34c at 0x0209f34c (symbols.txt: data_0209f34c).
 * The `1 << n` shift is load-bearing.
 */
extern s32 data_0209f34c; /* 0x0209f34c */

void _ZN5Event6SetBitEj(u32 bit)
{
    data_0209f34c |= 1 << bit;
}
