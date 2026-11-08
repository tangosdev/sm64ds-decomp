#ifndef OV007_TEXTURE_H
#define OV007_TEXTURE_H

#include "types.h"

/* Reconstructed layout and name, not recovered class identity (no RTTI, no
 * vtable; plain C helpers). The 0x2c-byte texture record func_ov007_020c15ec
 * fills and func_ov007_020c1448 / Texture_BindPalette bind for drawing
 * (unit020c1404);
 * func_ov007_020c4684 and func_ov007_020ca5f0 read its size. */
typedef struct Ov007Texture {
    /* 0x00: bit 0 = has texels (Texture_LoadTexels loads them),
     * bit 1 = has a palette (Texture_LoadPalette loads it) */
    u32 flags;
    u32 fmt;             /* 0x04: BMD texture format */
    const void *texSrc;  /* 0x08: texel data */
    u32 texSize;         /* 0x0c: texel bytes */
    u32 texAddr;         /* 0x10: VRAM slot the texels were loaded at */
    u32 sizeS;           /* 0x14: log2(width / 8) */
    u32 sizeT;           /* 0x18: log2(height / 8) */
    const void *plttSrc; /* 0x1c: palette data */
    u32 plttSize;        /* 0x20: palette bytes, colour count << 1 */
    u32 plttAddr;        /* 0x24: VRAM slot the palette was loaded at */
    u16 plttColorCount;  /* 0x28 */
} Ov007Texture;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007Texture_size_must_be_0x2c[sizeof(Ov007Texture) == 0x2c ? 1 : -1];
#endif

#endif
