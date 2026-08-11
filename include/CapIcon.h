#ifndef CAPICON_H
#define CAPICON_H

#include "types.h"

/* The cap marker a CapEnemy carries -- the icon shown on the enemy wearing
 * Mario's cap, and the entry that links it into the game's list of stolen caps.
 *
 * The ROM's own name for this class is `dCapIcon_c`: `_ZTI10dCapIcon_c` and
 * `_ZTS10dCapIcon_c` live in ov001 at 0x020ad450/0x020ad458, which is also
 * where its destructor func_ov001_020ab3a0 is. Kept as CapIcon here to match
 * how this tree names the rest of the family.
 *
 * The layout is the one _ZN8CapEnemyC2Ev.c already carried as a local shadow;
 * the size is what _ZN8CapEnemyD2Ev.c asserted as `char capIcon[0x1c]`, and it
 * closes CapEnemy exactly at 0x180.
 */
struct CapIcon {
    u32 *vtable;              /* 0x00 */
    u32  actor;               /* 0x04 */
    u32  objID;               /* 0x08 */
    u32  next;                /* 0x0c */
    u32  prev;                /* 0x10 */
    u32  arrayIndex;          /* 0x14 */
    u8   character;           /* 0x18 */
    u8   unk19;               /* 0x19 */
    u8   unk1a;               /* 0x1a */
    u8   unk1b;               /* 0x1b */
};

typedef char CapIcon_size_must_be_0x1c[sizeof(CapIcon) == 0x1c ? 1 : -1];

#endif /* CAPICON_H */
