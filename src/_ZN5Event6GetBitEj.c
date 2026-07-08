/* Event::GetBit(u32 bit) at 0x02029ee0
 * Free function in namespace Event (no `this`). Returns nonzero if bit `bit`
 * is set in the global event bitfield EVENT_FIELD. Declared in SM64DS_2.h:
 *   namespace Event { u32 GetBit(u32 bit); ... }
 *
 * Reloc resolves to EVENT_FIELD at 0x0209f34c (symbols.txt: EVENT_FIELD).
 * The `1 << n` shift is load-bearing.
 */

typedef int s32;
typedef unsigned int u32;

extern s32 EVENT_FIELD; /* 0x0209f34c */

s32 _ZN5Event6GetBitEj(u32 bit)
{
    return EVENT_FIELD & (1 << bit);
}
