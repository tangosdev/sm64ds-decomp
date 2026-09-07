//cpp
#include "types.h"
// @symbol func_ov002_020b2c44
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"
struct Mtx43 { Fix12i a[12]; };

extern "C" {
void Vec3_Asr(struct Vector3 *d, struct Vector3 *s, int sh);
void Matrix4x3_FromTranslation(struct Mtx43 *m, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 ang);
void Matrix4x3_ApplyInPlaceToRotationZ(void *m, s16 ang);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 ang);
void Matrix4x3_FromRotationY(void *m, int angle);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *mtx, Fix12i a, Fix12i b, u32 u);
}

extern struct Mtx43 data_020a0e68;
extern unsigned char data_0209f2d8;

extern "C" void func_ov002_020b2c44(char *c)
{
    struct Vector3 pos;
    struct Vector3 v;
    int b;

    Vec3_Asr(&v, (struct Vector3 *)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16 *)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(s16 *)(c + 0x90));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(c + 0x8c));
    *(struct Mtx43 *)(c + 0xf0) = data_020a0e68;

    b = (data_0209f2d8 == 1);
    if (b) {
        Matrix4x3_FromRotationY(c + 0x33c, *(s16 *)(c + 0x8e));
        *(int *)(c + 0x360) = *(int *)(c + 0x5c) >> 3;
        *(int *)(c + 0x364) = *(int *)(c + 0x60) >> 3;
        *(int *)(c + 0x368) = *(int *)(c + 0x64) >> 3;
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x314, c + 0x33c, 0x96000, 0x320000, 0xf);
        return;
    }

    {
        int r5;
        int r4;

        dBgCh_Gnd rg;
        pos.x = *(int *)(c + 0x5c);
        pos.y = *(int *)(c + 0x60);
        pos.z = *(int *)(c + 0x64);
        pos.y = pos.y + 0x14000;
        rg.SetObjAndPos(pos, (dActor_c*)c);
        rg.DetectClsn();

        r5 = *(int *)(c + 0x60) - rg.clsnY;
        if (r5 <= 0x1000)
            r5 = 0x1000;
        r4 = 0x96000 - (int)(((long long)r5 * 0x180 + 0x800) >> 12);
        if (r4 < 0xa000)
            r4 = 0xa000;

        Matrix4x3_FromRotationY(c + 0x33c, *(s16 *)(c + 0x8e));
        *(int *)(c + 0x360) = *(int *)(c + 0x5c) >> 3;
        *(int *)(c + 0x364) = *(int *)(c + 0x60) >> 3;
        *(int *)(c + 0x368) = *(int *)(c + 0x64) >> 3;
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x314, c + 0x33c, r4, r5 + 0x28000, 0xf);
    }
}
