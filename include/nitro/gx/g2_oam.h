#ifndef NITRO_GX_G2_OAM_H
#define NITRO_GX_G2_OAM_H

/*
 * NitroSDK's OAM attribute setters (nitro/gx/g2_oam.h). G2_SetOBJMode rewrites
 * the OBJ mode bits of attr0 through the attr01 word and the colour parameter
 * of attr2. A caller that only changes the mode passes the record's own colour
 * parameter back. dScMg3DEsp_c reads it as oam->cParam (a word load of
 * attr2/attr3 shifted left 16 and right 28); d_s_mg_smartball.cpp's local
 * getter masks the attr2 halfword instead, and the two spellings do not
 * compile alike.
 *
 * Only the setters a source uses are declared here. Like any inline body in a
 * header, including this one can move compiler-numbered @NNN symbols in the
 * including TU, so adopting it is matching work.
 */

#include "nitro/gx/gxcommon.h"

typedef enum {
    GX_OAM_MODE_NORMAL = 0,
    GX_OAM_MODE_XLU = 1,
    GX_OAM_MODE_OBJWND = 2,
    GX_OAM_MODE_BITMAPOBJ = 3
} GXOamMode;

#define GX_OAM_ATTR01_MODE_SHIFT 10
#define GX_OAM_ATTR01_MODE_MASK 0x00000c00
#define GX_OAM_ATTR2_CPARAM_SHIFT 12
#define GX_OAM_ATTR2_CPARAM_MASK 0xf000

static inline void G2_SetOBJMode(GXOamAttr *oam, GXOamMode mode, int cParam) {
    oam->attr01 = (oam->attr01 & ~GX_OAM_ATTR01_MODE_MASK) | (mode << GX_OAM_ATTR01_MODE_SHIFT);
    oam->attr2 =
        (u16)((oam->attr2 & ~GX_OAM_ATTR2_CPARAM_MASK) | (cParam << GX_OAM_ATTR2_CPARAM_SHIFT));
}

#endif /* NITRO_GX_G2_OAM_H */
