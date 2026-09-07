//cpp
// @symbol func_ov002_020af4ec
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"

extern "C" {
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* shadow, void* mtx, int height, int rad, unsigned int x);
void func_ov002_020af4ec(void* self);
}

void func_ov002_020af4ec(void* self)
{
    int rad;
    char* c = (char*)self;
    int height;
    struct Vector3 v2;
    struct Vector3 v1;

    if ((unsigned)(*(int*)(c + 0x384) - 0xb) <= 1) {
        Matrix4x3_FromRotationY(c + 0x31c, *(short*)(c + 0x8e));
        *(int*)(c + 0x340) = *(int*)(c + 0x5c) >> 3;
        *(int*)(c + 0x344) = *(int*)(c + 0x60) >> 3;
        *(int*)(c + 0x348) = *(int*)(c + 0x64) >> 3;
    } else {
        Vec3_Asr(&v1, (struct Vector3*)(c + 0x5c), 3);
        Matrix4x3_FromTranslation(c + 0x31c, v1.x, v1.y, v1.z);
    }

    if (*(unsigned char*)(c + 0x38e) == 0) return;

    if ((unsigned)(*(int*)(c + 0x384) - 0xb) <= 1) {
        height = 0x50000;
        rad = 0x50000;
    } else if (!_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144)) {
        int y = *(int*)(c + 0x60);
        int z = *(int*)(c + 0x64);
        int adjustedY;
        int x = *(int*)(c + 0x5c);
        adjustedY = y + 0x28000;
        v2.x = x;
        v2.y = adjustedY;
        v2.z = z;
        dBgCh_Gnd rg;
        rg.SetObjAndPos(v2, 0);
        rad = v2.y;
        if (rg.DetectClsn()) {
            rad = rg.clsnY;
        }
        rad = *(int*)(c + 0x60) - rad;
        if (rad <= 0x1000) rad = 0x1000;
        height = (*(int*)(c + 0x114) - 0xa000) * 2 - (int)(((long long)rad * 0x180 + 0x800) >> 12);
        if (height < 0xa000) height = 0xa000;
        rad += 0x3c000;
    } else {
        rad = 0x3c000;
        height = (*(int*)(c + 0x114) - 0xa000) * 2;
    }

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x350, c + 0x31c, height, rad, 0xf);
}
