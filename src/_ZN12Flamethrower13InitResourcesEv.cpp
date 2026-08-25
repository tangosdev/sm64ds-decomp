//cpp
// @symbol _ZN12Flamethrower13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Flamethrower.h"
#pragma opt_strength_reduction off
struct Vec3 { int x, y, z; };
extern "C" void MulVec3Mat4x3(struct Vec3 *v, void *m, struct Vec3 *dst);
extern "C" void Vec3_Add(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern "C" void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(void *self, struct Vec3 *pos, int fix, int t, unsigned int a, unsigned int b);

extern signed char data_0209f2f8;

int Flamethrower::InitResources()
{
    int count;
    int i;
    int im1;
    int zero;
    int flag;
    char *p6;
    char *p5;
    char *p4;
    struct Vec3 in;
    struct Vec3 dst;
    struct Vec3 sum;
    struct Vec3 sum2;

    count = 0xc;
    if (data_0209f2f8 == 0xc) count = 9;
    Matrix4x3_FromRotationXYZExt(((char *)this) + 0x434, mAngleX, mAngleY, mAngleZ);
    zero = 0;
    flag = 0;
    i = 0;
    if (i < count) {
        p6 = ((char *)this);
        p5 = ((char *)this) + 0x3a4;
        p4 = ((char *)this) + 0xd4;
        do {
        im1 = i - 1;
        in.x = zero;
        in.y = zero;
        in.z = zero;
        dst.x = zero;
        dst.y = zero;
        dst.z = zero;
        in.z = data_ov095_02136fb0[i];
        MulVec3Mat4x3(&in, ((char *)this) + 0x434, &dst);
        if (im1 < 0) {
            Vec3_Add(&sum, (struct Vec3 *)((char *)&mPosX), &dst);
            *(int *)(p6 + 0x3a4) = sum.x;
            *(int *)(p6 + 0x3a8) = sum.y;
            *(int *)(p6 + 0x3ac) = sum.z;
        } else {
            Vec3_Add(&sum2, (struct Vec3 *)(((char *)this) + 0x3a4 + im1 * 0xc), &dst);
            *(int *)(p6 + 0x3a4) = sum2.x;
            *(int *)(p6 + 0x3a8) = sum2.y;
            *(int *)(p6 + 0x3ac) = sum2.z;
        }
        {
            short v = data_ov095_02136f80[i];
            _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(p4, (struct Vec3 *)p5, v * 0x14, v * 0x28, 0x200002, flag);
        }
        p6 += 0xc;
        p5 += 0xc;
        p4 += 0x3c;
        i++;
        } while (i < count);
    }
    return 1;
}
