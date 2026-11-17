//cpp
#include "OAM.h"
#include "OamAnimation.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/gx/gxcommon.h"
#include "Matrix2x2.h"

/* Draw an OamAnimation's current frame at (x, y). OamAnimation.h's names
 * are kept although this reader contradicts two of them: `tile` holds the
 * current frame's OamAttr list (func_0202044c copies it from the frame
 * table), and `affine` gates drawing at all. `screen` picks the engine
 * (0 main, 1 sub, 2 both, the main copy 0xc0 lower). */
void func_0202048c(OamAnimation *anim, int x, int y) {
    if (anim->affine == 0)
        return;
    if (anim->tile == 0)
        return;

    int scaleX = anim->scaleX;
    if (scaleX == 0x1000 && anim->scaleY == 0x1000 && (u16)anim->rotation == 0) {
        int mode = anim->screen;
        if (mode == 2) {
            OAM::Render(0, (OamAttr *)anim->tile, x, y + 192, anim->palette, anim->priority, 0x1000,
                        0x1000, 0, -1);
            OAM::RenderSub((OamAttr *)anim->tile, x, y, anim->palette, anim->priority);
            return;
        }
        if (mode == 1) {
            OAM::RenderSub((OamAttr *)anim->tile, x, y, anim->palette, anim->priority);
            return;
        }
        OAM::Render(0, (OamAttr *)anim->tile, x, y, anim->palette, anim->priority, 0x1000, 0x1000,
                    0, -1);
        return;
    }

    int idx = ((u16)anim->rotation) >> 4;
    int scaleY = anim->scaleY;
    short cosVal = FX_CosIdx(idx);
    short sinVal = FX_SinIdx(idx);

    Matrix2x2 m;
    m.m00 = (cosVal * scaleX + 0x800) >> 12;
    m.m01 = (sinVal * scaleX + 0x800) >> 12;
    m.m10 = -((sinVal * scaleY + 0x800) >> 12);
    m.m11 = (cosVal * scaleY + 0x800) >> 12;

    /* The first OamAttr's first word is attr0 | attr1 << 16: bits 28 and 29
       are attr1's horizontal and vertical flip. They are folded into the
       matrix here, because in an affine sprite those bits pick the matrix. */
    if (((GXOamAttr *)anim->tile)->attr01 & 0x10000000) {
        m.m00 = -m.m00;
        m.m01 = -m.m01;
    }
    if (((GXOamAttr *)anim->tile)->attr01 & 0x20000000) {
        m.m10 = -m.m10;
        m.m11 = -m.m11;
    }

    int mode = anim->screen;
    if (mode == 2) {
        OAM::Render(false, (OamAttr *)anim->tile, x, y + 192, anim->palette, anim->priority, &m);
        OAM::Render(true, (OamAttr *)anim->tile, x, y, anim->palette, anim->priority, &m);
        return;
    }
    if (mode == 1) {
        OAM::Render(true, (OamAttr *)anim->tile, x, y, anim->palette, anim->priority, &m);
        return;
    }
    OAM::Render(false, (OamAttr *)anim->tile, x, y, anim->palette, anim->priority, &m);
}
