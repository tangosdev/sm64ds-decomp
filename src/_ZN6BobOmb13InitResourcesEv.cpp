//cpp
// @symbol _ZN6BobOmb13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * This file used to carry a `struct Obj` restating BobOmb's whole layout and
 * cast `this` to it for every access. BobOmb.h now derives from Enemy and
 * declares the dozen fields that shadow was hiding, so the cast is gone.
 *
 * param1's low three bits pick the variant: 2 is the one that starts inert --
 * it sets the collision volume's `hit` bit, clears a behaviour flag and leaves
 * unk_108 clear -- 4 also starts clear, and everything else starts live.
 *
 * Both Init calls keep extern "C" declarations with scalar slots: they carry
 * Fix12<int> BY VALUE, which mwccarm passes differently at the call site, so
 * spelling the true types breaks the byte match -- notes/mwccarm-codegen.md 6az.
 */
#include "BobOmb.h"
#include "SharedFilePtr.h"

/* ModelBase::SetFile is declared void in include/ModelBase.h -- which is what its
   own matched definition compiles as -- but the ROM leaves DoSetFile's int in r0
   and this call site reads it, so the value-returning entry keeps the mangled
   spelling until that signature is settled. */
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, BMD_File *f, int a, int b);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *, Actor* a, Fix12i r, Fix12i h, unsigned int d, unsigned int e);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *, Actor* a, Fix12i b, Fix12i c, Vector3_16* d, Fix12i e);
extern "C" void func_ov102_0214c0b8(void* c);

extern SharedFilePtr data_ov102_0214e9c0;
extern SharedFilePtr data_ov102_0214e9c8;
extern SharedFilePtr data_ov002_0210d9e0;

struct S48 { int a[12]; };
extern S48 data_02082128;

int BobOmb::InitResources()
{
    BMD_File* bmd;

    Animation::LoadFile(data_ov102_0214e9c0);
    Animation::LoadFile(data_ov102_0214e9c8);
    bmd = (BMD_File*)Model::LoadFile(data_ov002_0210d9e0);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, bmd, 1, -1) == 0)
        return 0;
    if (mShadowModel.InitCylinder() == 0)
        return 0;

    unk_3f5 = (unsigned char)(param1 & 7);
    unk_3ec = 0x2000;
    func_ov102_0214c0b8(this);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(&mCylinderClsn, this, 0x3c000, 0x50000, 0x200004, 0xa6d380);
    mTerminalVelocity = -0x37000;

    if (unk_3f5 == 2) {
        /* The add sits INSIDE the integer cast, which is load-bearing: not
           interchangeable with `mCylinderClsn.flags |= 2` or `mFlags &= ~1u`,
           though 0x128 is that field (+0x18) and 0xb0 is mFlags. */
        *(unsigned int*)(((int)this + 0x128)) |= 2;
        *(unsigned int*)(((int)this + 0xb0)) &= ~1u;
        unk_108 = 0;
    } else if (unk_3f5 == 4) {
        unk_108 = 0;
    } else {
        unk_108 = 1;
    }

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    *(S48 *)unk_394 = data_02082128;
    unk_3e8 = 0;
    unk_3ea = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    unk_390 = 0;
    unk_3f2 = 0;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    mWithMeshClsn.StartDetectingWater();
    unk_3f3 = 1;
    /* Actor does not name 0xc8 yet -- it is padding between unk_0c4 and
       mAreaId -- so this one stays an offset rather than growing a field on a
       header 200+ files include. */
    *(int *)((char *)this + 0xc8) = 0;
    unk_3e0 = 2;
    unk_3f6 = 0;
    unk_3f0 = mAngleY;
    return 1;
}
