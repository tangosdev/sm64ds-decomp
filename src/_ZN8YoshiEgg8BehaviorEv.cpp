//cpp
// @symbol _ZN8YoshiEgg8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "YoshiEgg.h"
extern "C" {
struct Vec3 { int x, y, z; };
extern int data_020a0e68;
extern void _ZN9Animation7AdvanceEv(void *c);
extern void MulVec3Mat4x3(void *in, void *mtx, void *out);
extern void Vec3_Add(void *out, void *a, void *b);
extern void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned int op, int z);
extern void func_ov002_020edca4(void *c);
}

int YoshiEgg::Behavior()
{
    Vec3 vin;
    Vec3 vmid;
    Vec3 vout;
    func_ov002_020ed684(((char *)this));
    _ZN9Animation7AdvanceEv((char *)&mAnimation);
    if (unk_3f0 != 1) {
        if (*(unsigned char *)(*(char **)((char *)&mPlayer) + 0x6f5) < 0xa) {
            short *ang;
            vin.z = 0;
            vin.z = -0x68000;
            vin.x = 0;
            vin.y = 0;
            vmid.x = 0;
            vmid.y = 0;
            vmid.z = 0;
            ang = (short *)(((int)*(char **)((char *)&mPlayer) + 0x8c) & 0xFFFFFFFFFFFFFFFF);
            Matrix4x3_FromRotationZXYExt(&data_020a0e68, ang[0], ang[1], ang[2]);
            MulVec3Mat4x3(&vin, &data_020a0e68, &vmid);
            Vec3_Add(&vout, *(char **)((char *)&mPlayer) + 0x5c, &vmid);
            mPosX = vout.x;
            mPosY = vout.y;
            mPosZ = vout.z;
        }
        _ZN9ModelBase12ApplyOpacityEj(((char *)this) + 0x300, *(unsigned char *)(*(char **)((char *)&mPlayer) + 0x6f5), 0);
    } else {
        _ZN9ModelBase12ApplyOpacityEj(((char *)this) + 0x300, 0x1f, 0);
    }
    func_ov002_020ed998(((char *)this));
    func_ov002_020ed7f8(((char *)this));
    {
        unsigned char idx = unk_420;
        if (*(unsigned char *)(((char *)this) + idx + 0x421) != 0) {
            unsigned char *cp = (unsigned char *)(((int)((char *)this) + 0x420) & 0xFFFFFFFFFFFFFFFF);
            *cp = *cp + 1;
        }
    }
    if (unk_420 >= 5) {
        func_ov002_020edca4(((char *)this));
    }
    return 1;
}
