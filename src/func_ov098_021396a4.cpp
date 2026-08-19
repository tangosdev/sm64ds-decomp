//cpp
// @symbol func_ov098_021396a4
/* recovered: shared common types */
#include "common.h"

struct dBgCh_Gnd { char buf[0x68 - 0x18]; };
extern "C" {
extern void _ZN9dBgCh_GndC1Ev(struct dBgCh_Gnd *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(struct dBgCh_Gnd *self, const struct Vector3 *v, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(struct dBgCh_Gnd *self);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mtx, int fix, int t1, int t2, unsigned int n);
extern void _ZN9dBgCh_GndD1Ev(struct dBgCh_Gnd *self);
void func_ov098_021396a4(void *self);
}

void func_ov098_021396a4(void *self)
{
    char *c = (char*)self;
    struct dBgCh_Gnd rg;
    struct Vector3 v;
    int r5;
    int r4;

    v.x = *(int*)(c + 0x5c);
    v.y = *(int*)(c + 0x60);
    v.z = *(int*)(c + 0x64);
    v.y -= 0xa000;
    _ZN9dBgCh_GndC1Ev(&rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, 0);
    *(int*)(c + 0x5ec) = v.y;
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg)) {
        *(int*)(c + 0x5ec) = *(int*)((char*)&rg + 0x44);
    }
    r5 = *(int*)(c + 0x60) - *(int*)(c + 0x5ec);
    if (r5 <= 0x1000) r5 = 0x1000;
    r4 = 0x64000 - (int)(((long long)r5 * 0x180 + 0x800) >> 12);
    if (r4 < 0xa000) r4 = 0xa000;
    Matrix4x3_FromRotationY(c + 0x530, *(short*)(c + 0x8e));
    *(int*)(c + 0x554) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x558) = (*(int*)(c + 0x60) - 0x14000) >> 3;
    *(int*)(c + 0x55c) = *(int*)(c + 0x64) >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x508, c + 0x530, r4, r5 + 0x28000, r4, 0xf);
    _ZN9dBgCh_GndD1Ev(&rg);
}
