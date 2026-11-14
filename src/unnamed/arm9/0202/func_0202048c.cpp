//cpp
#include "OAM.h"
#include "OamAnimation.h"
#include "nitro/fx/fx_trig.h"

extern "C" {
struct Matrix2x2;
}

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
            OAM::Render(0, (OamAttr *)anim->tile, x, y + 0xc0, anim->palette, anim->priority,
                        0x1000, 0x1000, 0, -1);
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

    int m[4];
    m[0] = (cosVal * scaleX + 0x800) >> 12;
    m[1] = (sinVal * scaleX + 0x800) >> 12;
    m[2] = -((sinVal * scaleY + 0x800) >> 12);
    m[3] = (cosVal * scaleY + 0x800) >> 12;

    /* The first OamAttr's first word is attr0 | attr1 << 16: bits 28 and 29
       are attr1's horizontal and vertical flip. They are folded into the
       matrix here, because in an affine sprite those bits pick the matrix. */
    if (*(int *)anim->tile & 0x10000000) {
        m[0] = -m[0];
        m[1] = -m[1];
    }
    if (*(int *)anim->tile & 0x20000000) {
        m[2] = -m[2];
        m[3] = -m[3];
    }

    int mode = anim->screen;
    if (mode == 2) {
        OAM::Render(false, (OamAttr *)anim->tile, x, y + 0xc0, anim->palette, anim->priority,
                    (Matrix2x2 *)m);
        OAM::Render(true, (OamAttr *)anim->tile, x, y, anim->palette, anim->priority,
                    (Matrix2x2 *)m);
        return;
    }
    if (mode == 1) {
        OAM::Render(true, (OamAttr *)anim->tile, x, y, anim->palette, anim->priority,
                    (Matrix2x2 *)m);
        return;
    }
    OAM::Render(false, (OamAttr *)anim->tile, x, y, anim->palette, anim->priority, (Matrix2x2 *)m);
}
