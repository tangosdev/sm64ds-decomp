//cpp
// @symbol func_ov002_020ed738
/* recovered: shared common types */
#include "common.h"

extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
extern void* _ZNK12WithMeshClsn14GetFloorResultEv(void* self);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, struct Vector3* out);
extern int func_02010844(void* unused, struct Vector3* v, short angle);
extern void _Z11UpdateAngleRssis(short* a, int b, int c, short d);
void func_ov002_020ed738(char* c) {
    int e4 = 0;
    int e6 = 0;
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c+0x144)) {
        struct Vector3 n;
        void* fr = _ZNK12WithMeshClsn14GetFloorResultEv(c+0x144);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)fr+4, &n);
        e4 = func_02010844(c, &n, *(short*)(c+0x8e));
        e6 = func_02010844(c, &n, (short)(*(short*)(c+0x8e) - 0x4000));
    } else {
        if (*(int*)(c+0x3f0) == 0) {
            short* p = *(short**)(c+0x38c);
            if (p != 0) {
                e4 = p[0x8c/2];
                e6 = p[0x90/2];
            }
        }
    }
    _Z11UpdateAngleRssis((short*)(c+0x8c), e4, 4, 0x1000);
    _Z11UpdateAngleRssis((short*)(c+0x90), e6, 4, 0x1000);
}
}
