#include "nitro/fx/fx.h"
typedef short s16;
typedef int s32;
typedef long long s64;

typedef struct {
    s16 x, y, z;
} Vec3s;

extern Vec3s data_02099fc4;
extern void func_02053380(void *a, Vec3s *b);

void func_0204cebc(void *c)
{
    Vec3s vecA;
    Vec3s vecB;

    vecA = data_02099fc4;
    func_02053380((char *)c + 0x3c, &vecB);

    if (vecA.x == vecB.x && vecA.y == vecB.y && vecA.z == vecB.z) {
        vecA.x = 0x1000;
        vecA.y = 0;
        vecA.z = 0;
    }

    *(s16 *)((char *)c + 0x5c) = (s16)(FX_MUL(vecB.y, vecA.z) - FX_MUL(vecB.z, vecA.y));
    *(s16 *)((char *)c + 0x5e) = (s16)(FX_MUL(vecB.z, vecA.x) - FX_MUL(vecB.x, vecA.z));
    *(s16 *)((char *)c + 0x60) = (s16)(FX_MUL(vecB.x, vecA.y) - FX_MUL(vecB.y, vecA.x));

    *(s16 *)((char *)c + 0x62) = (s16)(FX_MUL(vecB.y, *(s16 *)((char *)c + 0x60)) -
                                       FX_MUL(vecB.z, *(s16 *)((char *)c + 0x5e)));
    *(s16 *)((char *)c + 0x64) = (s16)(FX_MUL(vecB.z, *(s16 *)((char *)c + 0x5c)) -
                                       FX_MUL(vecB.x, *(s16 *)((char *)c + 0x60)));
    *(s16 *)((char *)c + 0x66) = (s16)(FX_MUL(vecB.x, *(s16 *)((char *)c + 0x5e)) -
                                       FX_MUL(vecB.y, *(s16 *)((char *)c + 0x5c)));
}
