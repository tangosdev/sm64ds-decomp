//cpp
#include "nitro/gx/g3imm.h"
#include "nitro/gx/g3imm_cmd.h"



/* Sets up the untextured stroke polygons func_ov007_020c19cc then draws:
 * no texture, vertex colour `color`, and a polygon attribute of both faces
 * shown, polygon ID 6 and alpha `alpha`. The colour goes to REG_COLOR as
 * the int it arrives as: G3_Color's u16 parameter adds a truncation the
 * cartridge does not have. */
void func_ov007_020c4fcc(int color, int alpha) {
    G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, GX_TEXSIZE_S8, GX_TEXSIZE_T8,
                     GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
    REG_COLOR = color;
    REG_POLYGON_ATTR = 0x060000c0 | (alpha << 16);
}

