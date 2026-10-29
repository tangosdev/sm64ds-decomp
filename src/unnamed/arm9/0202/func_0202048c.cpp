//cpp
#include "OAM.h"
#include "OamAnimation.h"

extern "C" {
struct Matrix2x2;
}
typedef int Fix12i;

extern "C" int OamRender10_func_0202048c(int show, void *attr, int a, int b, int c, int d, Fix12i e,
                                         Fix12i f, int g, int h);

extern short data_02082214[];

/* Draw an OamAnimation's current frame at (a1, a2). OamAnimation.h's names
 * are kept although this reader contradicts two of them: `tile` holds the
 * current frame's OamAttr list (func_0202044c copies it from the frame
 * table), and `affine` gates drawing at all. `screen` picks the engine
 * (0 main, 1 sub, 2 both, the main copy 0xc0 lower). */
extern "C" void func_0202048c(char* a0, int a1, int a2)
{
    OamAnimation *anim = (OamAnimation *)a0;

    if (anim->affine == 0)
        return;
    if (anim->tile == 0)
        return;

    int f08 = anim->scaleX;
    if (f08 == 0x1000 && anim->scaleY == 0x1000 && (u16)anim->rotation == 0) {
        int mode = anim->screen;
        if (mode == 2) {
            OAM::Render(0, (OamAttr *)anim->tile, a1, a2 + 0xc0, anim->palette, anim->priority,
                        0x1000, 0x1000, 0, -1);
            OAM::RenderSub((OamAttr *)anim->tile, a1, a2, anim->palette, anim->priority);
            return;
        }
        if (mode == 1) {
            OAM::RenderSub((OamAttr *)anim->tile, a1, a2, anim->palette, anim->priority);
            return;
        }
        OAM::Render(0, (OamAttr *)anim->tile, a1, a2, anim->palette, anim->priority, 0x1000, 0x1000,
                    0, -1);
        return;
    }

    int idx = ((u16)anim->rotation) >> 4;
    int f0c = anim->scaleY;
    int i0 = idx * 2;
    short tb = data_02082214[i0 + 1];
    short ta = data_02082214[i0];

    int m[4];
    m[0] = (tb * f08 + 0x800) >> 12;
    m[1] = (ta * f08 + 0x800) >> 12;
    m[2] = -((ta * f0c + 0x800) >> 12);
    m[3] = (tb * f0c + 0x800) >> 12;

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
        OAM::Render(false, (OamAttr *)anim->tile, a1, a2 + 0xc0, anim->palette, anim->priority,
                    (Matrix2x2 *)m);
        OAM::Render(true, (OamAttr *)anim->tile, a1, a2, anim->palette, anim->priority,
                    (Matrix2x2 *)m);
        return;
    }
    if (mode == 1) {
        OAM::Render(true, (OamAttr *)anim->tile, a1, a2, anim->palette, anim->priority,
                    (Matrix2x2 *)m);
        return;
    }
    OAM::Render(false, (OamAttr *)anim->tile, a1, a2, anim->palette, anim->priority,
                (Matrix2x2 *)m);
}
