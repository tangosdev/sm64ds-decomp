#ifndef OV007_SPRITE_H
#define OV007_SPRITE_H

#include "types.h"

/* Reconstructed layout and name, not recovered class identity (no RTTI, no
 * vtable; plain C helpers). The 0x88-byte screen-space textured quad that
 * func_ov007_020c44e4 allocates and initialises and func_ov007_020c44c4 frees.
 * Field roles come from its readers: func_ov007_020c4388 (screen -> world),
 * func_ov007_020c43bc (world size), func_ov007_020c42f8 (transform),
 * func_ov007_020c421c (draw + drop shadow) and func_ov007_020c4684 (quad). */
typedef struct Ov007Sprite {
    s32 drawFlags;   /* 0x00: c4684: 1|2 plain quad, 2 corner colours; c421c: 4 shadow; 8 tilemap */
    s32 screenX;     /* 0x04: c4388 -> func_ov007_020bfe4c centres it on 0x80000 (128 px) */
    s32 screenY;     /* 0x08: c4388 -> func_ov007_020bfe4c centres it on 0x60000 (96 px) */
    u16 width;       /* 0x0c: pixels; c43bc scales it, c4684 uses it as the texel width */
    u16 height;      /* 0x0e */
    s32 depth;       /* 0x10: c4388 view z; c43bc scales width/height by |depth| */
    Vector3 pos;     /* 0x14: c4388 writes the world position, c42f8 translates by it */
    s32 worldWidth;  /* 0x20: c43bc result; c4684 quad width */
    s32 worldHeight; /* 0x24 */
    Vector3 posOffset;   /* 0x28: c42f8 adds it to pos */
    u16 rotX;            /* 0x34: c42f8 rotation angles */
    u16 rotY;            /* 0x36 */
    u16 rotZ;            /* 0x38 */
    char pad_3a[2];      /* 0x3a */
    s32 scaleX;          /* 0x3c: c42f8 MTX_SCALE */
    s32 scaleY;          /* 0x40 */
    u16 color;           /* 0x44: c421c -> c4684 COLOR */
    u16 cornerColors[4]; /* 0x46: c4684 per-vertex COLOR when drawFlags & 2 */
    u8 polygonId;        /* 0x4e: c421c -> c4684 POLYGON_ATTR bits 24-29 */
    char pad_4f;         /* 0x4f */
    s32 alpha;           /* 0x50: Q12; c421c passes (alpha >> 12) & 0xff as POLYGON_ATTR alpha */
    s32 polygonAttr;     /* 0x54: c4684 ORs it into POLYGON_ATTR */
    u16 frameCount;      /* 0x58: c44e4 sizes frames[] by it */
    u16 frame;           /* 0x5a: c4684 draws frames[frame] */
    void **frames;       /* 0x5c: texture records (mgr textures[] entries) */
    u16 repeatU;         /* 0x60: c4684 texture repeat count; 1 means use width */
    u16 repeatV;         /* 0x62: 1 means use height */
    s32 texRepeat;       /* 0x64: func_ov007_020c1448 `repeat` */
    s32 texFlip;         /* 0x68: func_ov007_020c1448 `flip` */
    s32 texColor0;       /* 0x6c: func_ov007_020c1448 `color0` */
    s16 texS;            /* 0x70: c4684 texel origin */
    s16 texT;            /* 0x72 */
    s32 shadowScaleX;    /* 0x74: c421c second, black, offset pass: medium-confidence "shadow" */
    s32 shadowScaleY;    /* 0x78 */
    u8 shadowAlphaDrop;  /* 0x7c: subtracted from the alpha for the shadow pass */
    u8 shadowPolygonId;  /* 0x7d */
    s16 shadowOffset[3]; /* 0x7e: c421c passes it as the shadow pass's vector */
    u16 *tilemap;        /* 0x84: c4684 tile map when drawFlags & 8 */
} Ov007Sprite;

typedef char Ov007Sprite_size_must_be_0x88[sizeof(Ov007Sprite) == 0x88 ? 1 : -1];

#endif
