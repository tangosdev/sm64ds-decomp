//cpp
// @symbol func_ov065_02119fe8
/* recovered: shared common types */
#include "common.h"
typedef short s16;

struct dBgCh_Gnd { char buf0[0x14]; int floor[12]; char buf1[0x50-0x14-0x30]; };

extern "C" void Matrix4x3_FromRotationZXYExt(void* m, int x, int y, int z);
extern "C" void MulVec3Mat4x3(Vector3* in, void* m, Vector3* out);
extern "C" void AddVec3(Vector3* a, Vector3* b, Vector3* c);
extern "C" void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd* self);
extern "C" void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd* self, const Vector3& v, void* actor);
extern "C" int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd* self);
extern "C" void Matrix4x3_FromRotationY(void* m, int angle);
extern "C" void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void* self, void* sm, void* mtx, int f, int a, int b, unsigned int c);
extern "C" void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd* self);

extern void* data_020a0e68;

extern "C" void func_ov065_02119fe8(char* self) {
    Vector3 v1;
    Vector3 v2;
    Vector3 pos;
    dBgCh_Gnd rc;
    v1.y = 0;
    v1.y = -0x320000;
    v2.x = 0;
    v2.y = 0;
    v2.z = 0;
    v1.x = 0;
    v1.z = 0;
    Matrix4x3_FromRotationZXYExt(&data_020a0e68, *(s16*)(self + 0x8c), *(s16*)(self + 0x8e), *(s16*)(self + 0x90));
    MulVec3Mat4x3(&v1, &data_020a0e68, &v2);
    AddVec3(&v2, (Vector3*)(self + 0x5c), &v2);
    {
        int vy = v2.y;
        pos.x = v2.x;
        pos.z = v2.z;
        pos.y = vy;
        pos.y = vy - 0xc8000;
    }
    _ZN9dBgCh_GndC1Ev(&rc);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rc, pos, 0);
    *(int*)(self + 0x32c) = pos.y;
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rc))
        *(int*)(self + 0x32c) = rc.floor[(0x44 - 0x14) / 4];
    Matrix4x3_FromRotationY(self + 0x358, *(s16*)(self + 0x8e));
    *(int*)(self + 0x37c) = v2.x >> 3;
    *(int*)(self + 0x380) = *(int*)(self + 0x32c) >> 3;
    *(int*)(self + 0x384) = v2.z >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        self, self + 0x330, self + 0x358, 0x12c000, 0x12c000, 0x78000, 0xf);
    _ZN9dBgCh_GndD1Ev(&rc);
}
