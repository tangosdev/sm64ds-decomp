//cpp
// @symbol func_ov006_020ea914
/* recovered: five-point trailing-segment OAM draw loop; the trivial ~Vec2 and
 * `#pragma opt_strength_reduction off` are both load-bearing. */
#include "types.h"

struct Vec2 { s32 x; s32 y; ~Vec2() {} };

extern "C" {
extern s16 data_02082214[];
extern void *data_ov006_02142018[];
extern void *data_ov006_021375c0[];
void func_0203d388(Vec2 *v, s32 angle);
void func_0203d680(Vec2 *out, Vec2 *in, s32 s);
s32 func_ov006_020ebb40(char *self, s32 i);
s32 func_ov006_020ebc08(char *self, s32 i);
s32 func_ov006_020eb7b0(char *self);
s32 func_02053200(s32 a);
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(s32 a, void *b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h);
}

#pragma opt_strength_reduction off

extern "C" void func_ov006_020ea914(char *self)
{
    s32 i;
    s16 rot;
    s16 ang;
    s32 A;
    s32 B;
    s32 a2i;
    s32 px;
    s32 py;
    s32 res;
    s32 j;

    Vec2 pos;
    Vec2 va;
    Vec2 vb;
    Vec2 vout;
    Vec2 pts[5];

    pts[4].x = 0xb4000;
    pts[4].y = 0x80000;
    for (j = 3; j >= 1; j--) {
        pts[j].x = pts[j + 1].x;
        pts[j].y = pts[j + 1].y;
        pts[j].x = pts[j].x - (((s32 *)(self + j * 4))[0x13] + ((j + 1) << 13));
    }
    pts[0].x = pts[1].x;
    pts[0].y = pts[1].y;
    pts[0].x = pts[0].x - (*(s32 *)(self + 0x4c) + 0x2000);

    for (i = 0, A = 0, B = 0; i < 5; i++) {
        rot = -0x4000;
        va.y = 0x1000;
        va.x = 0;
        pos.x = pts[i].x;
        pos.y = pts[i].y;
        ang = (s16)(*(s16 *)(self + 0x84) - A);
        func_0203d388(&va, -0x4000);
        if (*(u8 *)(self + 0x94) == 0) {
            if (i == 0) {
                rot += data_02082214[((u16)ang >> 4) * 2] >> 1;
            } else {
                s32 v = data_02082214[((u16)ang >> 4) * 2];
                rot += (s16)(((s64)v * (0x1800 - B) + 0x800) >> 12);
            }
        }
        func_0203d680(&vout, &va, data_02082214[((u16)(ang * 2) >> 4) * 2]);
        pos.x = pos.x + vout.x;
        pos.y = pos.y + vout.y;
        res = func_ov006_020ebb40(self, i);
        px = pos.y >> 12;
        py = pos.x >> 12;
        a2i = func_ov006_020ebc08(self, i);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, data_ov006_02142018[a2i],
            py, px, res, -1, 0x1000, (u16)rot);
        if (i == 0) {
            if (*(u8 *)(self + 0x94) != 0) {
                if (func_ov006_020eb7b0(self) != 0) {
                    vb.x = 0x2000;
                    vb.y = -0x18000;
                    func_0203d388(&vb, rot);
                    vb.x = vb.x + pos.x;
                    vb.y = vb.y + pos.y;
                    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, data_ov006_021375c0[0],
                        vb.x >> 12, vb.y >> 12, -1, -1,
                        func_02053200((data_02082214[((u16)ang >> 4) * 2] >> 2) + 0xc00), (u16)rot);
                }
            }
        }
        A = A + 0x1800;
        B = B + 0x200;
    }
}
