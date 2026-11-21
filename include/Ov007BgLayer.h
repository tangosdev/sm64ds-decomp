#ifndef OV007_BG_LAYER_H
#define OV007_BG_LAYER_H

#include "types.h"

/* Reconstructed layout and name, not recovered class identity (no RTTI, no
 * vtable; plain C helpers). The 0x38-byte BG layer record that
 * func_ov007_020c0aa8 allocates and func_ov007_020c0a44 resets. Field roles
 * come from its readers: func_ov007_020c03b0 (shows the list's layers),
 * func_ov007_020c076c (screen upload and scroll), func_ov007_020c0638
 * (rebuilds screen from source) and func_ov007_020c0384 (list append). */
typedef struct Ov007BgLayer {
    /* 0x00: BG 0..3; picks the BGnCNT/BGnHOFS registers and the screen loader
     * (func_ov007_020c0a44 resets it to -1, which no list shows) */
    int bg;
    int priority; /* 0x04: written into BGnCNT bits 0-1 */
    int visible;  /* 0x08: func_ov007_020c03b0 shows the layer only while it is 1 */
    /* 0x0c: func_ov007_020c0638 sets it after rebuilding screen;
     * func_ov007_020c076c re-uploads screen while it is set, then clears it */
    int dirty;
    void *screen;     /* 0x10: uploaded to BG screen VRAM */
    void *source;     /* 0x14: func_ov007_020c0638 rebuilds screen from it */
    int screenOffset; /* 0x18: the byte offset into BG screen VRAM */
    u16 width;        /* 0x1c: in tiles, the screen's row length */
    u16 height;       /* 0x1e: in tiles */
    int fineScrollX;  /* 0x20: Q12; func_ov007_020c0638 keeps the sub-tile part of x, negated */
    int fineScrollY;  /* 0x24 */
    int scrollX;      /* 0x28: Q12; added to fineScrollX for BGnHOFS */
    int scrollY;      /* 0x2c */
    struct Ov007BgLayer *next; /* 0x30 */
    struct Ov007BgLayer *prev; /* 0x34 */
} Ov007BgLayer;

typedef char Ov007BgLayer_size_must_be_0x38[sizeof(Ov007BgLayer) == 0x38 ? 1 : -1];

/* The 0x10-byte BG layer list func_ov007_020c0604 builds (the display
 * record's bgListMain/bgListSub). */
typedef struct Ov007BgList {
    int engines;        /* 0x00: &1 main engine, &2 sub */
    int mask;           /* 0x04: the BGs to show (func_ov007_020c03b0) */
    Ov007BgLayer *head; /* 0x08 */
    Ov007BgLayer *tail; /* 0x0c */
} Ov007BgList;

#endif
