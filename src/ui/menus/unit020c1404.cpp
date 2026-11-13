//cpp
/* unit020c1404 -- ov007 unit 0x020c1404..0x020c19cc: 13 functions, .text only.
 *
 * The title menu's textures. ov007 is the title and file-select menu
 * program that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or
 * vtable, so no class name is claimed and the file keeps the unit<addr>
 * name.
 *
 * What the code does. A texture is one 0x2c-byte Ov007Texture record:
 * Texture_Create allocates and fills one from a BMD texture and
 * palette pair (func_ov007_020b2308), Texture_GetSizes working out its
 * texel and palette byte sizes, and func_ov007_020c1620 frees one.
 * func_ov007_020c14b8 uploads a record's texels and palette into VRAM.
 * Each comes from a bump allocator over one region: four bss words hold
 * the next free texture slot (data_ov007_02104bdc) and the region's end
 * (data_ov007_02104be4), and the next free palette slot
 * (data_ov007_02104be8) and that region's end (data_ov007_02104be0).
 * Texture_InitRegions (called from func_ov007_020bd4a0) sizes both regions
 * from the VRAM banks mapped to texture and palette use and starts each
 * at the given offset; func_ov007_020c1804 and 020c17f4 read the two next
 * slots back. func_ov007_020c1448 and Texture_BindPalette bind a record for
 * drawing, writing its TEXIMAGE_PARAM and PLTT_BASE.
 *
 * Edges: the four bss words 0x02104bdc..0x02104be8 are referenced by these
 * functions and nothing else in the overlay. func_ov007_020c1448 and
 * Texture_BindPalette touch no data; their callers (func_ov007_020c4684,
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
 * The functions keep C linkage. The Texture_ names are coined from the
 * bodies, after the in-tree Ov007Texture (include/Ov007Texture.h); the
 * other six keep their address names for now.
 */
#include "types.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "Ov007Texture.h"
#include "nitro/gx/g3imm.h"
#include "nitro/os/system.h"
#include "Ov007MenuMem.h"

extern "C" {


extern u32 data_ov007_02104bdc; /* next free texture slot */
extern u32 data_ov007_02104be0; /* end of the palette region */
extern u32 data_ov007_02104be4; /* end of the texture region */
extern u32 data_ov007_02104be8; /* next free palette slot */

void func_ov007_020c15ec(Ov007Texture *tex);
void Texture_GetSizes(int fmt, int width, int height, int colorCount, u32 *texSize, u32 *plttSize);

/* Resets both regions to start at 0: the texture region's end from the
 * texture VRAM banks GX_GetBankForTex reports (one 0x20000-byte bank per
 * bit), the palette region's end from the palette banks GX_GetBankForTexPltt
 * reports. */
void Texture_ResetRegions(void) {
    int texBanks = GX_GetBankForTex();
    int plttBanks = GX_GetBankForTexPltt();

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

/* Sizes both regions and starts them at the given VRAM offsets. */
void Texture_InitRegions(int textureOffset, int paletteOffset) {
    Texture_ResetRegions();
    data_ov007_02104bdc = textureOffset;
    data_ov007_02104be8 = paletteOffset;
}

/* The next free texture slot. */
int func_ov007_020c1804(void) {
    return data_ov007_02104bdc;
}

/* The next free palette slot. */
int func_ov007_020c17f4(void) {
    return data_ov007_02104be8;
}

/* Texel and palette byte sizes of a width x height texture of format fmt
 * with colorCount palette colours; the direct formats (0, 5 and 7) are
 * given none. */
void Texture_GetSizes(int fmt, int width, int height, int colorCount, u32 *texSize, u32 *plttSize) {
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

/* Allocates and fills a texture record for texels of format fmt at texSrc,
 * (8 << sizeS) by (8 << sizeT), with an optional palette of plttColorCount
 * colours at plttSrc. */
Ov007Texture *Texture_Create(int fmt, int texSrc, int sizeS, int sizeT, int plttSrc,
                             unsigned short plttColorCount) {
    Ov007Texture *tex = (Ov007Texture *)func_ov007_020c3df4(0, sizeof(Ov007Texture));
    int width = 8 << sizeS;
    int height = 8 << sizeT;
    func_ov007_020c15ec(tex);
    tex->fmt = fmt;
    tex->texSrc = (const void *)texSrc;
    tex->sizeS = sizeS;
    tex->sizeT = sizeT;
    tex->plttSrc = (const void *)plttSrc;
    tex->plttColorCount = plttColorCount;
    if (texSrc != 0)
        tex->flags |= 1;
    if (plttSrc != 0)
        tex->flags |= 2;
    Texture_GetSizes(fmt, width, height, plttColorCount, &tex->texSize, &tex->plttSize);
    return tex;
}
}


/* Frees a texture record. Built with long_calls, so the tail call is the
 * pooled `ldr ip,[pc]; bx ip` form; bracketed because the pragma is
 * positional. */
#pragma long_calls on
extern "C" {
void func_ov007_020c1620(void *tex) {
    func_ov007_020c3d1c(tex);
}
}

#pragma long_calls off
extern "C" {

/* Clears a texture record. */
void func_ov007_020c15ec(Ov007Texture *tex) {
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

/* Loads a record's texels at the next free texture slot, if it has any;
 * an overflowing region is fatal. */
void Texture_LoadTexels(Ov007Texture *tex) {
    if (!(tex->flags & 1))
        return;

    if (data_ov007_02104bdc + tex->texSize <= data_ov007_02104be4) {
        GX_BeginLoadTex();
        GX_LoadTex(tex->texSrc, data_ov007_02104bdc, tex->texSize);
        GX_EndLoadTex();

        tex->texAddr = data_ov007_02104bdc;
        data_ov007_02104bdc += tex->texSize;
    } else {
        OS_Terminate();
    }
}

/* Loads a record's palette at the next free palette slot, if it has one;
 * an overflowing region is fatal. */
void Texture_LoadPalette(Ov007Texture *tex) {
    if (!(tex->flags & 2))
        return;

    if (data_ov007_02104be8 + tex->plttSize <= data_ov007_02104be0) {
        GX_BeginLoadTexPltt();
        GX_LoadTexPltt(tex->plttSrc, data_ov007_02104be8, tex->plttSize);
        GX_EndLoadTexPltt();

        tex->plttAddr = data_ov007_02104be8;
        data_ov007_02104be8 += tex->plttSize;
    } else {
        OS_Terminate();
    }
}

/* Loads a record's texels and palette into VRAM. */
void func_ov007_020c14b8(Ov007Texture *tex) {
    Texture_LoadTexels(tex);
    Texture_LoadPalette(tex);
}

/* Binds a record's texels for drawing: TEXIMAGE_PARAM gets its VRAM slot,
 * format and size with the given coordinate source (texGen), repeat, flip
 * and colour-0 bits. No record selects no texture. */
void func_ov007_020c1448(Ov007Texture *self, u32 texGen, u32 repeat, u32 flip, u32 color0) {
    if (self != 0) {
        G3_TexImageParam((GXTexFmt)self->fmt, (GXTexGen)texGen, (GXTexSizeS)self->sizeS,
                         (GXTexSizeT)self->sizeT, (GXTexRepeat)repeat, (GXTexFlip)flip,
                         (GXTexPlttColor0)color0, self->texAddr);
    } else {
        reg_G3_TEXIMAGE_PARAM = 0x40000000;
    }
}

/* Binds a record's palette for drawing: PLTT_BASE gets its VRAM slot, in
 * 8-byte units for 4-colour textures (format 2) and 16-byte units
 * otherwise. No record writes 0. */
void Texture_BindPalette(Ov007Texture *tex) {
    if (tex) {
        G3_TexPlttBase(tex->plttAddr, (GXTexFmt)tex->fmt);
    } else {
        reg_G3_TEXPLTT_BASE = 0;
    }
}

}
 // extern "C"
