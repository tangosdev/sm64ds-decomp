//cpp
/* unit020c1404 -- ov007 unit 0x020c1404..0x020c19cc: 13 functions, .text only.
 *
 * The title menu's textures. ov007 is the title and file-select menu
 * program that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or
 * vtable, so no class name is claimed and the file keeps the unit<addr>
 * name.
 *
 * What the code does. A texture is one 0x2c-byte Ov007Texture record:
 * func_ov007_020c162c allocates and fills one from a BMD texture and
 * palette pair (func_ov007_020b2308), func_ov007_020c16dc working out its
 * texel and palette byte sizes, and func_ov007_020c1620 frees one.
 * func_ov007_020c14b8 uploads a record's texels and palette into VRAM.
 * Each comes from a bump allocator over one region: four bss words hold
 * the next free texture slot (data_ov007_02104bdc) and the region's end
 * (data_ov007_02104be4), and the next free palette slot
 * (data_ov007_02104be8) and that region's end (data_ov007_02104be0).
 * func_ov007_020c1814 (called from func_ov007_020bd4a0) sizes both regions
 * from the VRAM banks mapped to texture and palette use and starts each
 * at the given offset; func_ov007_020c1804 and 020c17f4 read the two next
 * slots back. func_ov007_020c1448 and 020c1404 bind a record for drawing,
 * writing its TEXIMAGE_PARAM and PLTT_BASE.
 *
 * Edges: the four bss words 0x02104bdc..0x02104be8 are referenced by these
 * functions and nothing else in the overlay. func_ov007_020c1448 and
 * 020c1404 touch no data; their callers (func_ov007_020c4684,
 * func_ov007_020ca5f0) are outside the run, but both functions take the
 * record the rest of the unit builds. The previous function,
 * func_ov007_020c13f4, is a callee of func_ov007_020c121c's module; the
 * next one, func_ov007_020c19cc, references no ov007 data and is called
 * only from func_ov007_020c4dfc / 020c4e98 / 020c4f50. Both edges are
 * medium.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 */
#include "types.h"
#include "GX.h"

/* File-wide: func_ov007_020c1448 needs it (mwccarm defers codegen, so an
 * optimization pragma covers the whole unit), and every other function
 * here matches under it. */
#pragma opt_propagation off

extern "C" {

#define reg_G3_TEXIMAGE_PARAM (*(volatile u32 *)0x040004a8)
#define reg_G3_TEXPLTT_BASE (*(volatile u32 *)0x040004ac)

/* NitroSDK's G3_TexImageParam. */
static inline void G3_TexImageParam(u32 addr, u32 texFmt, u32 texGen, u32 sizeS, u32 sizeT,
                                    u32 repeat, u32 flip, u32 pltt0) {
    reg_G3_TEXIMAGE_PARAM = (u32)((addr >> 3) | (texFmt << 26) | (texGen << 30) | (sizeS << 20) |
                                  (sizeT << 23) | (repeat << 16) | (flip << 18) | (pltt0 << 29));
}

/* The 0x2c-byte texture record. */
typedef struct Ov007Texture {
    /* 0x00: bit 0 = has texels (func_ov007_020c1560 loads them),
     * bit 1 = has a palette (func_ov007_020c14d4 loads it) */
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

typedef char Ov007Texture_size_must_be_0x2c[sizeof(Ov007Texture) == 0x2c ? 1 : -1];

extern u32 data_ov007_02104bdc; /* next free texture slot */
extern u32 data_ov007_02104be0; /* end of the palette region */
extern u32 data_ov007_02104be4; /* end of the texture region */
extern u32 data_ov007_02104be8; /* next free palette slot */

extern void Crash(void);
extern int func_02053ec0(void);
extern int func_02053ed0(void);
extern char *func_ov007_020c3df4(int a, int b);
extern void func_ov007_020c3d1c(void *self);

void func_ov007_020c15ec(char *p);
void func_ov007_020c16dc(int fmt, int width, int height, int colorCount, u32 *texSize,
                         u32 *plttSize);

// @symbol func_ov007_020c184c
/* Resets both regions to start at 0: the texture region's end from the
 * texture VRAM banks func_02053ed0 reports (one 0x20000-byte bank per
 * bit), the palette region's end from the palette banks func_02053ec0
 * reports. */
void func_ov007_020c184c(void) {
    int texBanks = func_02053ed0();
    int plttBanks = func_02053ec0();

    data_ov007_02104bdc = 0;
    data_ov007_02104be4 = 0;
    data_ov007_02104be8 = 0;
    data_ov007_02104be0 = 0;

    switch (texBanks) {
    case 1:
    case 2:
    case 4:
    case 8:
        data_ov007_02104be4 = 0x20000;
        break;
    case 3:
    case 5:
    case 6:
    case 9:
    case 10:
    case 12:
        data_ov007_02104be4 = 0x40000;
        break;
    case 7:
    case 11:
    case 13:
    case 14:
        data_ov007_02104be4 = 0x60000;
        break;
    case 15:
        data_ov007_02104be4 = 0x80000;
        break;
    }

    switch (plttBanks) {
    case 0:
        break;
    case 0x20:
    case 0x40:
        data_ov007_02104be0 = 0x4000;
        break;
    case 0x60:
        data_ov007_02104be0 = 0x8000;
        break;
    case 0x10:
        data_ov007_02104be0 = 0x10000;
        break;
    case 0x30:
        data_ov007_02104be0 = 0x14000;
        break;
    case 0x70:
        data_ov007_02104be0 = 0x18000;
        break;
    }
}

// @symbol func_ov007_020c1814
/* Sizes both regions and starts them at the given VRAM offsets. */
void func_ov007_020c1814(int textureOffset, int paletteOffset) {
    func_ov007_020c184c();
    data_ov007_02104bdc = textureOffset;
    data_ov007_02104be8 = paletteOffset;
}

// @symbol func_ov007_020c1804
/* The next free texture slot. */
int func_ov007_020c1804(void) {
    return data_ov007_02104bdc;
}

// @symbol func_ov007_020c17f4
/* The next free palette slot. */
int func_ov007_020c17f4(void) {
    return data_ov007_02104be8;
}

// @symbol func_ov007_020c16dc
/* Texel and palette byte sizes of a width x height texture of format fmt
 * with colorCount palette colours; the direct formats (0, 5 and 7) are
 * given none. */
void func_ov007_020c16dc(int fmt, int width, int height, int colorCount, u32 *texSize,
                         u32 *plttSize) {
    switch (fmt) {
    case 0:
        *texSize = 0;
        *plttSize = 0;
        break;
    case 1:
        *texSize = width * height;
        *plttSize = colorCount << 1;
        break;
    case 2:
        *texSize = (unsigned)(width * height) >> 2;
        *plttSize = colorCount << 1;
        break;
    case 3:
        *texSize = (unsigned)(width * height) >> 1;
        *plttSize = colorCount << 1;
        break;
    case 4:
        *texSize = width * height;
        *plttSize = colorCount << 1;
        break;
    case 5:
        *texSize = 0;
        *plttSize = 0;
        break;
    case 6:
        *texSize = width * height;
        *plttSize = colorCount << 1;
        break;
    case 7:
        *texSize = 0;
        *plttSize = 0;
        break;
    default:
        break;
    }
}

// @symbol func_ov007_020c162c
/* Allocates and fills a texture record for texels of format fmt at texSrc,
 * (8 << sizeS) by (8 << sizeT), with an optional palette of plttColorCount
 * colours at plttSrc. The record keeps the void * spelling its caller
 * func_ov007_020b2308 declares; `tex` is the typed view. */
void *func_ov007_020c162c(void *fmt, int texSrc, int sizeS, int sizeT, int plttSrc,
                          unsigned short plttColorCount) {
    char *r6 = func_ov007_020c3df4(0, sizeof(Ov007Texture));
    Ov007Texture *tex = (Ov007Texture *)r6;
    int width = 8 << sizeS;
    int height = 8 << sizeT;
    func_ov007_020c15ec(r6);
    tex->fmt = (u32)fmt;
    tex->texSrc = (const void *)texSrc;
    tex->sizeS = sizeS;
    tex->sizeT = sizeT;
    tex->plttSrc = (const void *)plttSrc;
    tex->plttColorCount = plttColorCount;
    if (texSrc != 0)
        tex->flags |= 1;
    if (plttSrc != 0)
        tex->flags |= 2;
    func_ov007_020c16dc((int)fmt, width, height, plttColorCount, &tex->texSize, &tex->plttSize);
    return r6;
}

// @symbol func_ov007_020c1620
/* Frees a texture record. Built with long_calls, so the tail call is the
 * pooled `ldr ip,[pc]; bx ip` form; bracketed because the pragma is
 * positional. */
#pragma long_calls on
void func_ov007_020c1620(void *tex) {
    func_ov007_020c3d1c(tex);
}
#pragma long_calls off

// @symbol func_ov007_020c15ec
/* Clears a texture record. */
void func_ov007_020c15ec(char *p) {
    Ov007Texture *tex = (Ov007Texture *)p;
    tex->flags = 0;
    tex->fmt = 0;
    tex->texSrc = 0;
    tex->sizeS = 0;
    tex->sizeT = 0;
    tex->texSize = 0;
    tex->texAddr = 0;
    tex->plttSrc = 0;
    tex->plttColorCount = 0;
    tex->plttSize = 0;
    tex->plttAddr = 0;
}

// @symbol func_ov007_020c1560
/* Loads a record's texels at the next free texture slot, if it has any;
 * an overflowing region is fatal. */
void func_ov007_020c1560(Ov007Texture *t) {
    if (!(t->flags & 1))
        return;

    if (data_ov007_02104bdc + t->texSize <= data_ov007_02104be4) {
        GX::BeginLoadTex();
        GX::LoadTex(t->texSrc, data_ov007_02104bdc, t->texSize);
        GX::EndLoadTex();

        t->texAddr = data_ov007_02104bdc;
        data_ov007_02104bdc += t->texSize;
    } else {
        Crash();
    }
}

// @symbol func_ov007_020c14d4
/* Loads a record's palette at the next free palette slot, if it has one;
 * an overflowing region is fatal. */
void func_ov007_020c14d4(Ov007Texture *t) {
    if (!(t->flags & 2))
        return;

    if (data_ov007_02104be8 + t->plttSize <= data_ov007_02104be0) {
        GX::BeginLoadTexPltt();
        GX::LoadTexPltt(t->plttSrc, data_ov007_02104be8, t->plttSize);
        GX::EndLoadTexPltt();

        t->plttAddr = data_ov007_02104be8;
        data_ov007_02104be8 += t->plttSize;
    } else {
        Crash();
    }
}

// @symbol func_ov007_020c14b8
/* Loads a record's texels and palette into VRAM. t keeps the void *
 * spelling its caller func_ov007_020bd1c0 declares. */
void func_ov007_020c14b8(void *t) {
    func_ov007_020c1560((Ov007Texture *)t);
    func_ov007_020c14d4((Ov007Texture *)t);
}

// @symbol func_ov007_020c1448
/* Binds a record's texels for drawing: TEXIMAGE_PARAM gets its VRAM slot,
 * format and size with the given coordinate source (texGen), repeat, flip
 * and colour-0 bits. No record selects no texture. */
void func_ov007_020c1448(Ov007Texture *self, u32 texGen, u32 repeat, u32 flip, u32 color0) {
    if (self != 0) {
        G3_TexImageParam(self->texAddr, self->fmt, texGen, self->sizeS, self->sizeT, repeat, flip,
                         color0);
    } else {
        reg_G3_TEXIMAGE_PARAM = 0x40000000;
    }
}

// @symbol func_ov007_020c1404
/* Binds a record's palette for drawing: PLTT_BASE gets its VRAM slot, in
 * 8-byte units for 4-colour textures (format 2) and 16-byte units
 * otherwise. No record writes 0. */
void func_ov007_020c1404(Ov007Texture *tex) {
    if (tex) {
        int t = (tex->fmt == 2) ? 1 : 0;
        reg_G3_TEXPLTT_BASE = tex->plttAddr >> (4 - t);
    } else {
        reg_G3_TEXPLTT_BASE = 0;
    }
}

} // extern "C"
