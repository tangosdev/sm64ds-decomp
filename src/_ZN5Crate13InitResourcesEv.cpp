//cpp
// @symbol _ZN5Crate13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Crate.h"
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(char* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(char* thiz, int f, int a, int b);
extern void _ZN11ShadowModel10InitCuboidEv(char* thiz);
extern int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(char* f);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(char* thiz, int f, char* m, int fix, short s, int blk);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(char* thiz, char* actor, int b, int d, void* v, int f);
extern void _ZN10dBgCh_Actr19StartDetectingWaterEv(char* thiz);
extern void Crate_SetState(char* c, int i);
}
extern char data_ov098_0213c4c8[];

int Crate::InitResources()
{
    char* f = 0;
    if (actorID == 0xc2) f = data_ov098_0213c4c8;
    if (f == 0) return 0;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mHomeAngleX = mAngleX;
    mHomeAngleY = mAngleY;
    mHomeAngleZ = mAngleZ;
    _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)&mModel, _ZN5Model8LoadFileER13SharedFilePtr(*(char**)(f)), 1, -1);
    _ZN11ShadowModel10InitCuboidEv((char*)&mShadowModel);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        (char*)&mMeshCollider, _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(char**)(f + 4)),
        (char*)&mClsnMat, 0x199, mAngleY, *(int*)(f + 8));
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_((char*)&mWithMeshClsn, ((char*)this), 0x28000, 0x28000, 0, 0);
    _ZN10dBgCh_Actr19StartDetectingWaterEv((char*)&mWithMeshClsn);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    Crate_SetState(((char*)this), 0);
    /* dBgActor_c's own generic 0xd0..0xd4 pad (include/dBgActor_c.h), not a
       Crate field -- reused here by raw offset, same idiom as
       ArmedRotatingPlatform's tail-padding field. */
    *(s32*)(((char*)this) + 0xd0) = 0;
    func_ov098_02138ce0(((char*)this));
    return 1;
}
