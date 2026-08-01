//cpp
#include "types.h"
extern "C" {
void _ZN11RaycastLineC1Ev(void* self);
void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* self, void* a, void* b, void* act);
int _ZN11RaycastLine10DetectClsnEv(void* self);
void _ZN11RaycastLineD1Ev(void* self);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, void* out);
}

extern s16 data_02082214[];

struct V3 {
    int x, y, z;
    void set(int x_, int y_, int z_) { x = x_; y = y_; z = z_; }
};

extern "C" int func_ov002_020c19d0(char* self, int arg1, int arg2) {
    V3 v1;
    V3 v2;
    V3 nrm;
    char rl[0x78];
    int angle = *(unsigned short*)(self + 0x8e);
    _ZN11RaycastLineC1Ev(rl);
    int k = angle >> 4;
    int cs = data_02082214[k * 2];
    int sn = data_02082214[k * 2 + 1];
    v1.set(*(int*)(self + 0x5c), *(int*)(self + 0x60) + (arg2 << 12), *(int*)(self + 0x64));
    v2.set(arg1 * cs + *(int*)(self + 0x5c), *(int*)(self + 0x60) + (arg2 << 12), arg1 * sn + *(int*)(self + 0x64));
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &v1, &v2, self);
    if (_ZN11RaycastLine10DetectClsnEv(rl) == 0) {
        _ZN11RaycastLineD1Ev(rl);
        return 0;
    }
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rl + 0x14, &nrm);
    if (nrm.y != 0) {
        _ZN11RaycastLineD1Ev(rl);
        return 0;
    }
    _ZN11RaycastLineD1Ev(rl);
    return 1;
}
