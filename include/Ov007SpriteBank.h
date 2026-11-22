#ifndef OV007_SPRITE_BANK_H
#define OV007_SPRITE_BANK_H

#include "types.h"

/* Reconstructed layouts and names, not recovered class identity: the ov007
 * menu's OBJ sprite banks. func_ov007_020c9688 builds one from a sprite
 * file, func_ov007_020c121c frees one, and func_ov007_020b6d40 keeps six
 * in the display record. */

/* One OAM entry of a cell: its three attribute halfwords, packed by
 * func_ov007_020c9688 and read by func_ov007_020c0eb0. */
typedef struct Ov007CellEntry {
    u16 attr0; /* 0x00 */
    u16 attr1; /* 0x02 */
    u16 attr2; /* 0x04 */
} Ov007CellEntry;

/* One 8-byte cell: `count` OAM entries. func_ov007_020c13f4 frees the
 * list. */
typedef struct Ov007Cell {
    Ov007CellEntry *oam; /* 0x00 */
    int count;           /* 0x04 */
} Ov007Cell;

/* One 0x10-byte cell animation, sized by func_ov007_020c13a4: a cell and a
 * duration per frame, which func_ov007_020c0d70 steps through. */
typedef struct Ov007CellAnim {
    Ov007Cell **frames; /* 0x00 */
    u16 *durations;     /* 0x04: ticks per frame */
    /* 0x08, medium confidence: func_ov007_020c0b78 adds the pair to the
     * node's screen x/y before drawing the frame's cells. Every animation
     * func_ov007_020c9688 builds has 0 for both. Unsigned: the reader's
     * ldrh zero-extends them, and s16 fields cost func_ov007_020c0b78 two
     * mismatches. The writer, func_ov007_020c13a4, stores them with strh
     * whatever its parameters' type is, so they are u16 too. */
    u16 offsetX;
    u16 offsetY;    /* 0x0a */
    u32 frameCount; /* 0x0c */
} Ov007CellAnim;

/* The 0x10-byte sprite bank func_ov007_020c12ac allocates: cellCount cells
 * and animCount animations. */
typedef struct Ov007SpriteBank {
    int cellCount;        /* 0x00 */
    Ov007Cell *cells;     /* 0x04 */
    int animCount;        /* 0x08 */
    Ov007CellAnim *anims; /* 0x0c */
} Ov007SpriteBank;

typedef char Ov007CellEntry_size_must_be_0x6[sizeof(Ov007CellEntry) == 0x6 ? 1 : -1];
typedef char Ov007Cell_size_must_be_0x8[sizeof(Ov007Cell) == 0x8 ? 1 : -1];
typedef char Ov007CellAnim_size_must_be_0x10[sizeof(Ov007CellAnim) == 0x10 ? 1 : -1];
typedef char Ov007SpriteBank_size_must_be_0x10[sizeof(Ov007SpriteBank) == 0x10 ? 1 : -1];

#endif
