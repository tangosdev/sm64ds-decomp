#ifndef NITRO_GX_G3IMM_CMD_H
#define NITRO_GX_G3IMM_CMD_H

#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/hw/registers.h"

/* NitroSDK's immediate-mode geometry commands (g3imm.h) for the matrix stack,
 * the vertex list, the buffer swap and the viewport, with the enums and
 * packing macros they take (g3.h). Bodies as in ntrtwl/NitroSDK
 * include/nitro/gx/g3imm.h, less its asserts; the ports are registers.h's
 * REG_* spelling of the SDK's reg_G3_*.
 *
 * Not in nitro/gx/g3imm.h: any function body there renumbers the pinned @NNN
 * of every file that includes it, ov075/d_s_entry_entry and
 * ov006/unit020cd744 among them (one added body moves both: measured with
 * tubuild verify). Only the files that issue these commands include this one.
 *
 * The SDK's GXRgb is spelled u16 in G3_Color. */

typedef enum {
    GX_MTXMODE_PROJECTION = 0,
    GX_MTXMODE_POSITION = 1,
    GX_MTXMODE_POSITION_VECTOR = 2,
    GX_MTXMODE_TEXTURE = 3
} GXMtxMode;

typedef enum {
    GX_BEGIN_TRIANGLES = 0,
    GX_BEGIN_QUADS = 1,
    GX_BEGIN_TRIANGLE_STRIP = 2,
    GX_BEGIN_QUAD_STRIP = 3
} GXBegin;

typedef enum { GX_SORTMODE_AUTO = 0, GX_SORTMODE_MANUAL = 1 } GXSortMode;
typedef enum { GX_BUFFERMODE_Z = 0, GX_BUFFERMODE_W = 1 } GXBufferMode;

/* GX_VECFX10 is gxcommon.h's: three 1.0.9 fixed-point components, ten bits
 * each, from fx16/fx32 values (12 fraction bits). */
#define GX_VECFX10(x, y, z)                                                   \
    ((u32)(((((x) >> 3) & 0x3ff) << 0) | ((((y) >> 3) & 0x3ff) << 10) | \
           ((((z) >> 3) & 0x3ff) << 20)))

/* GX_VECVTX10 is g3.h's: three 1.3.6 fixed-point components, ten bits each,
 * from fx16/fx32 values. */
#define GX_VECVTX10(x, y, z)                                                  \
    ((u32)(((((x) >> 6) & 0x3ff) << 0) | ((((y) >> 6) & 0x3ff) << 10) | \
           ((((z) >> 6) & 0x3ff) << 20)))

#define GX_FX16PAIR(a, b) ((u32)((u32)(u16)(a) | ((u32)(u16)(b) << 16)))
#define GX_FX16ST(x)      ((s16)((x) >> 8))
#define GX_ST(s, t)       ((u32)((u16)GX_FX16ST(s) | ((u16)GX_FX16ST(t) << 16)))

#define GX_PACK_MTXMODE_PARAM(mode)        ((u32)(mode))
#define GX_PACK_POPMTX_PARAM(num)          ((u32)(num))
#define GX_PACK_STOREMTX_PARAM(num)        ((u32)(num))
#define GX_PACK_COLOR_PARAM(rgb)           ((u32)(rgb))
#define GX_PACK_NORMAL_PARAM(x, y, z)      ((u32)GX_VECFX10((x), (y), (z)))
#define GX_PACK_TEXCOORD_PARAM(s, t)       (GX_ST((s), (t)))
#define GX_PACK_VTX10_PARAM(x, y, z)       ((u32)GX_VECVTX10((x), (y), (z)))
#define GX_PACK_VTXXZ_PARAM(x, z)          (GX_FX16PAIR((x), (z)))
#define GX_PACK_VTXYZ_PARAM(y, z)          (GX_FX16PAIR((y), (z)))
#define GX_PACK_BEGIN_PARAM(primitive)     ((u32)(primitive))
#define GX_PACK_SWAPBUFFERS_PARAM(am, zw)  ((u32)((am) | ((zw) << 1)))
#define GX_PACK_VIEWPORT_PARAM(x1, y1, x2, y2) \
    ((u32)((x1) | ((y1) << 8) | ((x2) << 16) | ((y2) << 24)))

static inline void G3_MtxMode(GXMtxMode mode) {
    REG_MTX_MODE = GX_PACK_MTXMODE_PARAM(mode);
}

static inline void G3_PushMtx(void) {
    REG_MTX_PUSH = 0;
}

static inline void G3_PopMtx(int num) {
    REG_MTX_POP = GX_PACK_POPMTX_PARAM(num);
}

static inline void G3_StoreMtx(int num) {
    REG_MTX_STORE = GX_PACK_STOREMTX_PARAM(num);
}

static inline void G3_Identity(void) {
    REG_MTX_IDENTITY = 0;
}

static inline void G3_Scale(fx32 x, fx32 y, fx32 z) {
    REG_MTX_SCALE = (u32)x;
    REG_MTX_SCALE = (u32)y;
    REG_MTX_SCALE = (u32)z;
}

static inline void G3_Translate(fx32 x, fx32 y, fx32 z) {
    REG_MTX_TRANS = (u32)x;
    REG_MTX_TRANS = (u32)y;
    REG_MTX_TRANS = (u32)z;
}

static inline void G3_Color(u16 rgb) {
    REG_COLOR = GX_PACK_COLOR_PARAM(rgb);
}

static inline void G3_Normal(fx16 x, fx16 y, fx16 z) {
    REG_NORMAL = GX_PACK_NORMAL_PARAM(x, y, z);
}

static inline void G3_TexCoord(fx32 s, fx32 t) {
    REG_TEXCOORD = GX_PACK_TEXCOORD_PARAM(s, t);
}

static inline void G3_Vtx(fx16 x, fx16 y, fx16 z) {
    REG_VTX_16 = GX_FX16PAIR(x, y);
    REG_VTX_16 = (u32)(u16)z;
}

static inline void G3_Vtx10(fx16 x, fx16 y, fx16 z) {
    REG_VTX_10 = GX_PACK_VTX10_PARAM(x, y, z);
}

static inline void G3_VtxXZ(fx16 x, fx16 z) {
    REG_VTX_XZ = GX_PACK_VTXXZ_PARAM(x, z);
}

static inline void G3_VtxYZ(fx16 y, fx16 z) {
    REG_VTX_YZ = GX_PACK_VTXYZ_PARAM(y, z);
}

static inline void G3_Begin(GXBegin primitive) {
    REG_BEGIN_VTXS = GX_PACK_BEGIN_PARAM(primitive);
}

static inline void G3_End(void) {
    REG_END_VTXS = 0;
}

static inline void G3_SwapBuffers(GXSortMode am, GXBufferMode zw) {
    REG_SWAP_BUFFERS = GX_PACK_SWAPBUFFERS_PARAM(am, zw);
}

static inline void G3_ViewPort(int x1, int y1, int x2, int y2) {
    REG_VIEWPORT = GX_PACK_VIEWPORT_PARAM(x1, y1, x2, y2);
}

#endif /* NITRO_GX_G3IMM_CMD_H */
