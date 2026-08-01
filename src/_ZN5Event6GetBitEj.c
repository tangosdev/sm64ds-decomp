#include "types.h"
/* Event::GetBit(u32 bit) at 0x02029ee0
 * Free function in namespace Event (no `this`). Returns nonzero if bit `bit`
 * is set in the global event bitfield data_0209f34c. Declared in SM64DS_2.h:
 *   namespace Event { u32 GetBit(u32 bit); ... }
 *
 * Reloc resolves to data_0209f34c at 0x0209f34c (symbols.txt: data_0209f34c).
 * The `1 << n` shift is load-bearing.
 */
extern s32 data_0209f34c; /* 0x0209f34c */

s32 _ZN5Event6GetBitEj(u32 bit)
{
    return data_0209f34c & (1 << bit);
}
