#ifndef NITRO_GX_G3IMM_LIGHT_H
#define NITRO_GX_G3IMM_LIGHT_H

#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/hw/registers.h"

/* NitroSDK's two light commands (g3imm.h), with the light-number enum and the
 * packing macros they take (g3.h). Bodies as in ntrtwl/NitroSDK
 * include/nitro/gx/g3imm.h, less its asserts; the ports are registers.h's
 * REG_* spelling of the SDK's reg_G3_*.
 *
 * Not in nitro/gx/g3imm_cmd.h, for the reason that one is not in g3imm.h:
 * every function body in a header renumbers the pinned @NNN of each file that
 * includes it. The files that set lights are mostly not the ones that send
 * vertices, so only they include this one.
 *
 * The SDK's GXRgb is spelled u16 in G3_LightColor, as in G3_Color. GX_RGB is
 * gxcommon.h's, kept here (as g3imm_cmd.h keeps GX_VECFX10) because that
 * header's GXOamAttr costs every includer four more @NNN numbers. */

typedef enum {
    GX_LIGHTID_0 = 0,
    GX_LIGHTID_1 = 1,
    GX_LIGHTID_2 = 2,
    GX_LIGHTID_3 = 3
} GXLightId;

/* A 15-bit BGR555 colour from 5-bit red, green and blue. */
#define GX_RGB(r, g, b) ((u16)(((r) << 0) | ((g) << 5) | ((b) << 10)))

#define GX_PACK_LIGHTVECTOR_PARAM(lightID, x, y, z) \
    ((u32)(((((x) >> 3) & 0x3ff) << 0) |            \
           ((((y) >> 3) & 0x3ff) << 10) |           \
           ((((z) >> 3) & 0x3ff) << 20) |           \
           ((lightID) << 30)))
#define GX_PACK_LIGHTCOLOR_PARAM(lightID, rgb) ((u32)(((lightID) << 30) | (rgb)))

static inline void G3_LightVector(GXLightId lightID, fx16 x, fx16 y, fx16 z) {
    REG_LIGHT_VECTOR = GX_PACK_LIGHTVECTOR_PARAM(lightID, x, y, z);
}

static inline void G3_LightColor(GXLightId lightID, u16 rgb) {
    REG_LIGHT_COLOR = GX_PACK_LIGHTCOLOR_PARAM(lightID, rgb);
}

#endif /* NITRO_GX_G3IMM_LIGHT_H */
