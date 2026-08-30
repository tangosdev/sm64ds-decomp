//cpp
#include "dBgCh_Lin.h"
extern "C" {
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
    int angle = *(unsigned short*)(self + 0x8e);
    dBgCh_Lin line;
    int k = angle >> 4;
    int cs = data_02082214[k * 2];
    int sn = data_02082214[k * 2 + 1];
    v1.set(*(int*)(self + 0x5c), *(int*)(self + 0x60) + (arg2 << 12), *(int*)(self + 0x64));
    v2.set(arg1 * cs + *(int*)(self + 0x5c), *(int*)(self + 0x60) + (arg2 << 12), arg1 * sn + *(int*)(self + 0x64));
    line.SetObjAndLine(*(Vector3*)&v1, *(Vector3*)&v2, (dActor_c*)self);
    if (!line.DetectClsn()) {
        return 0;
    }
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(&line.surface, &nrm);
    if (nrm.y != 0) {
        return 0;
    }
    return 1;
}
