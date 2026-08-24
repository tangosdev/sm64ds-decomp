//cpp
// @symbol _ZN14KnockDownPlank13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "KnockDownPlank.h"
#include "dBgCh_Gnd.h"

extern "C" {
void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
void _ZN11ShadowModel10InitCuboidEv(void* self);
void func_ov015_0211166c(char* t);
void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* fp);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* f, void* m, int fx, short s, void* b);
void Matrix4x3_FromRotationY(void* m, int angle);
void MulVec3Mat4x3(Vector3* v, void* m, Vector3* out);
void Vec3_Add(Vector3* out, Vector3* a, Vector3* b);
void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd* self);
void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd* self, Vector3* v, void* a);
int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd* self);
int IsStarCollectedInCurLevel(int starID);
void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd* self);
}

extern int data_ov015_0211497c;
extern int data_ov015_02114974;
extern int data_ov015_02113574;
extern int data_020a0e68;
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;

// 6f: keep constant live / flip coloring
#pragma opt_propagation off
int KnockDownPlank::InitResources()
{
    /* Dumb word storage, not a typed local: a dBgCh_Gnd local now
       synthesizes its constructor at the declaration, but the ROM constructs
       it after model setup -- so keep raw words and hand-call below. */
    u32 rg[sizeof(dBgCh_Gnd) / sizeof(u32)];
    Vector3 a, b, c, d;
    int zero, one;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(
        &mModel,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov015_0211497c),
        1, -1);
    _ZN11ShadowModel10InitCuboidEv(&mShadowModel);
    func_ov015_0211166c((char *)this);
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov015_02114974),
        &mClsnMat,
        0x1000,
        mAngleY,
        &data_ov015_02113574);

    mAngleY += 0x8000;
    zero = 0;
    a.z = -0xfa000; a.x = zero; a.y = zero; b.x = zero; b.y = zero; b.z = zero;

    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MulVec3Mat4x3(&a, &data_020a0e68, &b);
    Vec3_Add(&c, &b, (Vector3 *)&mPosX);
    c.y += 0x14000;
    _ZN9dBgCh_GndC1Ev((dBgCh_Gnd *)rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c((dBgCh_Gnd *)rg, &c, 0);
    mFrontFloorY = c.y;
    if (_ZN9dBgCh_Gnd10DetectClsnEv((dBgCh_Gnd *)rg) != 0)
        mFrontFloorY = ((dBgCh_Gnd *)rg)->clsnY;

    zero = 0;
    a.z = 0x32000; a.x = zero; a.y = zero; b.x = zero; b.y = zero; b.z = zero;

    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MulVec3Mat4x3(&a, &data_020a0e68, &b);
    Vec3_Add(&d, &b, (Vector3 *)&mPosX);

    one = 1;
    unk_378 = d.x;
    unk_37c = d.y;
    unk_380 = d.z;
    mOriginalPosY = mPosY;
    mKnockDir = (s8)one;

    if (data_0209f2f8 == 7 && (data_0209f220 == 1 || IsStarCollectedInCurLevel(one) == 0)
        && mPosY >= 0xdac000)
    {
        _ZN9dBgCh_GndD1Ev((dBgCh_Gnd *)rg);
        return 0;
    }
    _ZN9dBgCh_GndD1Ev((dBgCh_Gnd *)rg);
    return 1;
}
