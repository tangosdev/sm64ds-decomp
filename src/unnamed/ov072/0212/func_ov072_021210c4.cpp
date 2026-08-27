//cpp
// @symbol func_ov072_021210c4
/* recovered: shared common types */
#include "common.h"

struct dBgCh_Gnd { char buf[0x68 - 0x18]; };
extern "C" {
extern int _ZN6Player14IsFrontSlidingEv(void*);
extern int _ZN6Player17LostGrabbedObjectEv(void*);
extern void* _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(void*, void*, void*);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void _ZN9dBgCh_GndC1Ev(struct dBgCh_Gnd *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(struct dBgCh_Gnd *self, const struct Vector3 *v, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(struct dBgCh_Gnd *self);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *shadow, void *mtx, int fix, int t1, unsigned int t2);
extern void _ZN9dBgCh_GndD1Ev(struct dBgCh_Gnd *self);
extern char data_ov072_02122d3c[];


void func_ov072_021210c4(void *self);
}

void func_ov072_021210c4(void *self)
{
    char *c = (char*)self;
    int idx;
    void *res;
    struct dBgCh_Gnd rg;
    struct Vector3 v;
    int r5;
    int r4;
    long long tmp;

    if (*(void**)(c + 0x360) && ((*(int*)(c + 0xb0) & 0x4000) ? 1 : 0) && *(int*)(*(char**)(c + 0x360) + 0xc8)) {
        idx = 0;
        if (_ZN6Player14IsFrontSlidingEv(*(void**)(c + 0x360)) || _ZN6Player17LostGrabbedObjectEv(*(void**)(c + 0x360))) {
            idx = 1;
        }
        if (*(int*)(*(char**)(c + 0x360) + 8) == 2) {
            idx = (idx + 2) & 0xff;
        }
        res = _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(c, *(void**)(c + 0x360), data_ov072_02122d3c + idx * 0xc);
        *(struct Matrix4x3*)(c + 0xf0) = *(struct Matrix4x3*)res;
    } else {
        Matrix4x3_FromRotationY(c + 0xf0, *(short*)(c + 0x8e));
        *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
        *(int*)(c + 0x118) = *(int*)(c + 0x60) >> 3;
        *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;
    }

    if ((*(int*)(c + 0xb0) & 0x40000) ? 1 : 0) {
        return;
    }

    r5 = 0xb4000;
    r4 = 0x5a000;
    if (*(void**)(c + 0x360) && ((*(int*)(c + 0xb0) & 0x4000) ? 1 : 0)) {
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        v.y -= 0xa000;
        _ZN9dBgCh_GndC1Ev(&rg);
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, 0);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg)) {
            r5 = *(int*)(c + 0x60) - *(int*)((char*)&rg + 0x44);
            if (r5 < 0x1000) r5 = 0x1000;
            tmp = (long long)r5 * 0x180 + 0x800;
            r4 = r4 - (int)(tmp >> 12);
            if (r4 < 0x1000) r4 = 0x1000;
            if (r4 > 0x5a000) r4 = 0x5a000;
            r5 = r5 + 0x28000;
        }
        _ZN9dBgCh_GndD1Ev(&rg);
    }

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x138, c + 0xf0, r4, r5, 0xf);
}
