#ifndef NITRO_GX_G3IMM_H
#define NITRO_GX_G3IMM_H

#include "types.h"

/* NitroSDK's texture-parameter enums (gxcommon.h), packing macros (g3.h)
 * and immediate-mode inlines (g3imm.h), for the sources that bind a texture
 * through them. The parameters keep the SDK's enum types: with u32 or int
 * parameters the TEXIMAGE_PARAM pack schedules differently from the ROM
 * unless opt_propagation is off (notes/mwccarm-codegen.md 6di). */

typedef enum {
    GX_TEXFMT_NONE = 0,
    GX_TEXFMT_A3I5 = 1,
    GX_TEXFMT_PLTT4 = 2,
    GX_TEXFMT_PLTT16 = 3,
    GX_TEXFMT_PLTT256 = 4,
    GX_TEXFMT_COMP4x4 = 5,
    GX_TEXFMT_A5I3 = 6,
    GX_TEXFMT_DIRECT = 7
} GXTexFmt;

typedef enum {
    GX_TEXGEN_NONE = 0,
    GX_TEXGEN_TEXCOORD = 1,
    GX_TEXGEN_NORMAL = 2,
    GX_TEXGEN_VERTEX = 3
} GXTexGen;

typedef enum {
    GX_TEXSIZE_S8 = 0,
    GX_TEXSIZE_S16 = 1,
    GX_TEXSIZE_S32 = 2,
    GX_TEXSIZE_S64 = 3,
    GX_TEXSIZE_S128 = 4,
    GX_TEXSIZE_S256 = 5,
    GX_TEXSIZE_S512 = 6,
    GX_TEXSIZE_S1024 = 7
} GXTexSizeS;

typedef enum {
    GX_TEXSIZE_T8 = 0,
    GX_TEXSIZE_T16 = 1,
    GX_TEXSIZE_T32 = 2,
    GX_TEXSIZE_T64 = 3,
    GX_TEXSIZE_T128 = 4,
    GX_TEXSIZE_T256 = 5,
    GX_TEXSIZE_T512 = 6,
    GX_TEXSIZE_T1024 = 7
} GXTexSizeT;

typedef enum {
    GX_TEXREPEAT_NONE = 0,
    GX_TEXREPEAT_S = 1,
    GX_TEXREPEAT_T = 2,
    GX_TEXREPEAT_ST = 3
} GXTexRepeat;

typedef enum {
    GX_TEXFLIP_NONE = 0,
    GX_TEXFLIP_S = 1,
    GX_TEXFLIP_T = 2,
    GX_TEXFLIP_ST = 3
} GXTexFlip;

typedef enum { GX_TEXPLTTCOLOR0_USE = 0, GX_TEXPLTTCOLOR0_TRNS = 1 } GXTexPlttColor0;

#define reg_G3_TEXIMAGE_PARAM (*(volatile u32 *)0x040004a8)
#define reg_G3_TEXPLTT_BASE (*(volatile u32 *)0x040004ac)

#define GX_PACK_TEXIMAGE_PARAM(texFmt, texGen, s, t, repeat, flip, pltt0, addr)                \
    ((u32)(((addr) >> 3) | ((texFmt) << 26) | ((texGen) << 30) | ((s) << 20) | ((t) << 23) | \
           ((repeat) << 16) | ((flip) << 18) | ((pltt0) << 29)))

#define GX_PACK_TEXPLTTBASE_PARAM(addr, texFmt) \
    ((u32)((addr) >> (4 - ((texFmt) == GX_TEXFMT_PLTT4))))

static inline void G3_TexImageParam(GXTexFmt texFmt, GXTexGen texGen, GXTexSizeS s, GXTexSizeT t,
                                    GXTexRepeat repeat, GXTexFlip flip, GXTexPlttColor0 pltt0,
                                    u32 addr) {
    reg_G3_TEXIMAGE_PARAM = GX_PACK_TEXIMAGE_PARAM(texFmt, texGen, s, t, repeat, flip, pltt0, addr);
}

static inline void G3_TexPlttBase(u32 addr, GXTexFmt texFmt) {
    reg_G3_TEXPLTT_BASE = GX_PACK_TEXPLTTBASE_PARAM(addr, texFmt);
}

#endif
