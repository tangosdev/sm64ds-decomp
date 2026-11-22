#ifndef OV007_SPRITE_GROUP_H
#define OV007_SPRITE_GROUP_H

#include "types.h"
#include "Ov007OamNode.h"

/* Reconstructed layouts and names, not recovered class identity. */

/* The 0xc-byte sprite item func_ov007_020c40d4 builds around a fresh node
 * and func_ov007_020c40b4 frees. func_ov007_020c4090 resets the three
 * halfwords; no reader of them found. margin as in func_ov007_020c3fe4,
 * which shows the node only while it lies within margin pixels of the
 * screen. */
typedef struct Ov007SpriteItem {
    Ov007OamNode *node; /* 0x00 */
    short half4;        /* 0x04: 0 on reset */
    short half6;        /* 0x06: 0 on reset */
    short half8;        /* 0x08: -1 on reset */
    u8 margin;          /* 0x0a: in pixels */
} Ov007SpriteItem;

/* The 0x18-byte sprite group func_ov007_020c3f00 builds and
 * func_ov007_020c3e7c frees: a root node with one child per item. The two
 * words after the records have no reader found. */
typedef struct Ov007SpriteGroup {
    int *layout;             /* 0x00: [0] is the item count, [2] each item's animation count */
    Ov007OamNode *root;      /* 0x04: the items' parent (func_ov007_020c10b0) */
    Ov007SpriteItem **items; /* 0x08: layout[0] records */
    Ov007SpriteItem *extra;  /* 0x0c: one more record, also under root */
    int word10;              /* 0x10: -1 on build */
    int word14;              /* 0x14: 0 on build */
} Ov007SpriteGroup;

typedef char Ov007SpriteItem_size_must_be_0xc[sizeof(Ov007SpriteItem) == 0xc ? 1 : -1];
typedef char Ov007SpriteGroup_size_must_be_0x18[sizeof(Ov007SpriteGroup) == 0x18 ? 1 : -1];

#endif
