//cpp
// @symbol _ZN8YoshiEgg8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "YoshiEgg.h"
#include "Player.h"
extern "C" {
extern int data_020a0e68;
extern void MulVec3Mat4x3(void *in, void *mtx, void *out);
extern void Vec3_Add(void *out, void *a, void *b);
extern void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned int op, int z);
extern void func_ov002_020edca4(void *c);
}

int YoshiEgg::Behavior()
{
    Vector3 vin;
    Vector3 vmid;
    Vector3 vout;
    func_ov002_020ed684(this);
    mModelAnim.Advance();
    if (unk_3f0 != 1) {
        if (mPlayer->mOpacity < 0xa) {
            /* BOTH oddities below are load-bearing, measured one at a time.

               `ang` must stay a pointer: reading the three angles as
               mPlayer->mAngleX/Y/Z instead reloads mPlayer per field and
               changes the function's size.

               `vin.z` really is written twice. Dropping the dead first store
               also changes the size, so the ROM's own source had it. */
            s16 *ang;
            vin.z = 0;
            vin.z = -0x68000;
            vin.x = 0;
            vin.y = 0;
            vmid.x = 0;
            vmid.y = 0;
            vmid.z = 0;
            ang = &mPlayer->mAngleX;
            Matrix4x3_FromRotationZXYExt(&data_020a0e68, ang[0], ang[1], ang[2]);
            MulVec3Mat4x3(&vin, &data_020a0e68, &vmid);
            Vec3_Add(&vout, &mPlayer->mPosX, &vmid);
            mPosX = vout.x;
            mPosY = vout.y;
            mPosZ = vout.z;
        }
        _ZN9ModelBase12ApplyOpacityEj(&mModelAnim, mPlayer->mOpacity, 0);
    } else {
        _ZN9ModelBase12ApplyOpacityEj(&mModelAnim, 0x1f, 0);
    }
    func_ov002_020ed998(this);
    func_ov002_020ed7f8(this);
    if (unk_421[unk_420] != 0)
        unk_420++;
    if (unk_420 >= 5) {
        func_ov002_020edca4(this);
    }
    return 1;
}
