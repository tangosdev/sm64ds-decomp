#include "nitro/gx/g3imm_cmd.h"

/* Draw the unit quad in the XY plane (z = 0), with the texture repeated
 * 2^tileS by 2^tileT times. The particle draws pick it or func_0204c194 out of
 * data_02099fb4 by the definition's polygon reference plane; pret's SPL names
 * it SPLUtil_DrawXYPlane (that revision adds an offset). */
void func_0204c24c(int tileS, int tileT) {
    fx32 s = 0x1000 << tileS;
    fx32 t = 0x1000 << tileT;

    G3_Begin(GX_BEGIN_QUADS);
    G3_TexCoord(0, 0);
    G3_Vtx10(-0x1000, 0x1000, 0);
    G3_TexCoord(s, 0);
    G3_Vtx10(0x1000, 0x1000, 0);
    G3_TexCoord(s, t);
    G3_Vtx10(0x1000, -0x1000, 0);
    G3_TexCoord(0, t);
    G3_Vtx10(-0x1000, -0x1000, 0);
    G3_End();
}
