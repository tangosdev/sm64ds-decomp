//cpp
// @symbol _ZN6Rabbit13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_PathPtr.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Rabbit.h"
extern char data_ov085_021305d0;
extern int data_0209caa0[];
extern s8 data_0209f2f8;
extern int data_0209e650;

extern "C" {
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, void* actor, s32 a, s32 b, u32 c, u32 d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, s32 a, s32 b, void* v, void* w);
extern void _ZN7PathPtrC1Ev(void* thiz);
extern void _ZN7PathPtr6FromIDEj(void* thiz, u32 id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* thiz, void* v, u32 idx);
extern void func_ov085_0212bcc8(char* c);
extern void* _ZN5Actor13ClosestPlayerEv(void* c);
extern u32 RandomIntInternal(int* seed);
extern u8 NumStars(void);
extern void func_ov085_0212bc78(void* c, void* p);
}

int Rabbit::InitResources()
{
    char sp8[8];
    void* r0;
    void* r6;
    int r1;

    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_021305b8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_021305d0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_021305b0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_021305c8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_021305c0);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_021305d8);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x300, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_021305e0), 1, -1);
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel1);
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel2);

    unk_438 = param1 & 0xff;
    if (unk_438 == 0xff)
        unk_438 = 0;

    /* Actor declares param1 u32, but the ROM shifts these two with ASR, not
       LSR -- so this call site reads it signed. Without the casts the function
       comes out two words different; the flat header called 0x008 an s32,
       which is why this was invisible before the rebase. */
    mRabbitId = ((s32)param1 & 0xf00) >> 8;
    if (mRabbitId == 0xff)
        mRabbitId = 0;

    mCharacterId = ((s32)param1 & 0xf000) >> 0xc;
    if (mCharacterId == 0xf)
        mCharacterId = 0;

    r1 = mRabbitId;
    if (r1 != 7) {
        if (!(data_0209caa0[1] & 0x40000000))
            return 0;
    }

    if (r1 == 5 && mCharacterId == 0)
        goto check18;
    if (r1 == 1 && mCharacterId == 1)
        goto check18;
    if (r1 != 6)
        goto skip17;
    if (mCharacterId != 3)
        goto skip17;

check18:
    if (!(data_0209caa0[2] & 0x80000))
        return 0;

skip17:
    mVertAccel = -0x1000;
    mTerminalVelocity = -0x1e000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this) + 0x110, ((char*)this), 0x50000, 0x64000, 0xb00004, 0x9000);
    unk_45c = 0;
    mModelAnim.speed = 0x1000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x144, ((char*)this), 0x28000, 0x28000, 0, 0);
    _ZN7PathPtrC1Ev(sp8);
    _ZN7PathPtr6FromIDEj(sp8, unk_438);
    unk_448 = 1;
    _ZNK7PathPtr7GetNodeER7Vector3j(sp8, ((char*)this) + 0x5c, unk_448);
    unk_444 = _ZNK7PathPtr8NumNodesEv(sp8);
    func_ov085_0212bcc8(((char*)this));
    mEatingPlayer = 0;
    mScaleX = 0x1000;
    mScaleZ = mScaleX;
    mScaleY = mScaleZ;

    if (mRabbitId == 7) {
        if (data_0209caa0[1] & 0x40)
            return 0;
        unk_428 = 1;
        goto block_26;
    }

    r0 = _ZN5Actor13ClosestPlayerEv(((char*)this));
    if (r0 == 0)
        return 0;
    if (data_0209f2f8 != 0x32) {
        if (mCharacterId != *(u8*)((char*)r0 + 0x6d9))
            return 0;
    }

block_26:
    mColorVariant = mCharacterId + 1;
    if (mCharacterId == 3)
        mColorVariant = 0;

    if (mRabbitId == 7) {
        mColorVariant = 0;
        goto block_out;
    }

    r6 = _ZN5Actor13ClosestPlayerEv(((char*)this));
    if (r6 == 0)
        goto block_out;

    {
        int v;
        v = *(s32*)((char*)r6 + 8);
        if (data_0209f2f8 == 0x32)
            v = 1;
        if (func_02013890(mRabbitId, v) != 0 && data_ov085_021305ac < 8) {
            u32 rnd = RandomIntInternal(&data_0209e650) >> 8;
            if (NumStars() >= 0x51) {
                if (*(s32*)((char*)r6 + 8) == 3) {
                    if ((rnd & 0xf) == 0)
                        mColorVariant = 5;
                } else {
                    if ((rnd & 7) == 0)
                        mColorVariant = 5;
                }
            } else if (NumStars() >= 0x28) {
                if (*(s32*)((char*)r6 + 8) == 3) {
                    if ((rnd & 0x1f) == 0)
                        mColorVariant = 5;
                } else {
                    if ((rnd & 0xf) == 0)
                        mColorVariant = 5;
                }
            }
            if (mColorVariant == 5) {
                data_ov085_021305ac += 1;
                unk_429 = 1;
            }
        }
    }

block_out:
    if (data_0209f2f8 == 5) {
        if (mAreaId == 3)
            mFlags = 0x8280;
    }
    unk_468 = (mColorVariant << 1) + *(s32*)((char*)mModelAnim.data.materials + 0x20);
    func_ov085_0212bc78(((char*)this), &data_ov085_021306cc);
    return 1;
}
