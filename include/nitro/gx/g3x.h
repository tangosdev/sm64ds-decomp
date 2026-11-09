#ifndef NITRO_GX_G3X_H
#define NITRO_GX_G3X_H

#include "types.h"
#include "nitro/hw/registers.h"

/* NitroSDK's DISP3DCNT setters (g3x.h). Every write masks off bits 12 and 13,
 * the color-buffer underflow and polygon/vertex RAM overflow
 * acknowledgements, which a 1 would clear. The SDK's BOOL is spelled bool,
 * as G3X::SetClearColor and G3X::SetFog take it (include/G3X.h). Not in
 * g3imm.h: an inline body there renumbers ov075/d_s_entry_entry's pinned
 * @NNN (src/unnamed/ov007/func_ov007_020c4684.c). */

#define REG_G3X_DISP3DCNT_THS_MASK 0x0002
#define REG_G3X_DISP3DCNT_ABE_MASK 0x0008
#define REG_G3X_DISP3DCNT_AAE_MASK 0x0010
#define REG_G3X_DISP3DCNT_EME_MASK 0x0020
#define REG_G3X_DISP3DCNT_RO_MASK  0x1000
#define REG_G3X_DISP3DCNT_GO_MASK  0x2000
#define REG_G3X_DISP3DCNT_THS_SHIFT 1

typedef enum { GX_SHADING_TOON = 0, GX_SHADING_HIGHLIGHT = 1 } GXShading;

static inline void G3X_SetShading(GXShading shading) {
    REG_DISP3DCNT =
        (u16)((REG_DISP3DCNT & ~(REG_G3X_DISP3DCNT_THS_MASK | REG_G3X_DISP3DCNT_RO_MASK |
                                 REG_G3X_DISP3DCNT_GO_MASK)) |
              (shading << REG_G3X_DISP3DCNT_THS_SHIFT));
}

static inline void G3X_AlphaBlend(bool enable) {
    if (enable) {
        REG_DISP3DCNT =
            (u16)(REG_DISP3DCNT & ~(REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_GO_MASK) |
                  REG_G3X_DISP3DCNT_ABE_MASK);
    } else {
        REG_DISP3DCNT &= (u16) ~(REG_G3X_DISP3DCNT_ABE_MASK | REG_G3X_DISP3DCNT_RO_MASK |
                                 REG_G3X_DISP3DCNT_GO_MASK);
    }
}

static inline void G3X_AntiAlias(bool enable) {
    if (enable) {
        REG_DISP3DCNT =
            (u16)(REG_DISP3DCNT & ~(REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_GO_MASK) |
                  REG_G3X_DISP3DCNT_AAE_MASK);
    } else {
        REG_DISP3DCNT &= (u16) ~(REG_G3X_DISP3DCNT_AAE_MASK | REG_G3X_DISP3DCNT_RO_MASK |
                                 REG_G3X_DISP3DCNT_GO_MASK);
    }
}

static inline void G3X_EdgeMarking(bool enable) {
    if (enable) {
        REG_DISP3DCNT =
            (u16)(REG_DISP3DCNT & ~(REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_GO_MASK) |
                  REG_G3X_DISP3DCNT_EME_MASK);
    } else {
        REG_DISP3DCNT &= (u16) ~(REG_G3X_DISP3DCNT_EME_MASK | REG_G3X_DISP3DCNT_RO_MASK |
                                 REG_G3X_DISP3DCNT_GO_MASK);
    }
}

#endif
