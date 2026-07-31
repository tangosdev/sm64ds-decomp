// @symbol func_ov065_0211ac0c
/* recovered: shared common types */
#include "common.h"
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(void* a, void* b, void* c);
extern void AddVec3(void* a, void* b, void* c);
extern void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    int self, int sm, int mat, int s1, int s2, int s3, unsigned int j);

void func_ov065_0211ac0c(int c)
{
    struct Vector3 v1;
    struct Vector3 v2;
    int d;

    d = *(int*)(c + 0x60) - *(int*)(c + 0x338);
    if (d < 0) d = -d;
    if (d > 0x9c4000) return;
    if (d < 0xa000) return;

    v1.z = 0;
    v1.z = 0xffebb000;
    v2.x = 0;
    v2.y = 0;
    v2.z = 0;
    v1.x = 0;
    v1.y = 0;

    Matrix4x3_FromRotationY((void*)(c + 0x364), *(short*)(c + 0x8e));
    MulVec3Mat4x3(&v1, (void*)(c + 0x364), &v2);
    AddVec3(&v2, (void*)(c + 0x5c), &v2);

    *(int*)(c + 0x388) = v2.x >> 3;
    *(int*)(c + 0x38c) = (*(int*)(c + 0x338)) >> 3;
    *(int*)(c + 0x390) = v2.z >> 3;

    _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x33c, c + 0x364, 0xdc000, 0x32000, 0x28a000, 0xf);
}
