//cpp
// @symbol _ZN13TTC_MovingBar13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TTC_MovingBar.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *bmd, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
}
extern "C" {
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);
}
extern "C" {
extern void func_020393d4(int *p, int v);
extern void _ZN9dBgCh_GndC1Ev(void *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self, void *pos, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(void *self);
extern void _ZN9dBgCh_GndD1Ev(void *self);
}
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;


struct Vec3 { int x, y, z; };

int TTC_MovingBar::InitResources()
{
    struct Vec3 pos;
    char raycast[0x50];
    int i;

    if (actorID != 0x72) {
        if (actorID == 0x73)
            mVariant = 1;
    } else {
        mVariant = 0;
    }

    i = mVariant;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(
        &mModel,
        _ZN5Model8LoadFileER13SharedFilePtr(*(void **)(data_ov065_0211d35c + i * 0xc)),
        1, -1);

    _ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char *)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));

    i = mVariant;
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(void **)(data_ov065_0211d360 + i * 0xc)),
        &mClsnMat,
        0x199,
        mAngleY,
        *(void **)(data_ov065_0211d364 + i * 0xc));

    func_020393d4((int *)((char *)&(*(u8 *)&mMeshCollider)), (int)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y = pos.y - 0xa000;

    _ZN9dBgCh_GndC1Ev(raycast);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(raycast, &pos, (void *)0);
    mGroundY = pos.y;
    if (_ZN9dBgCh_Gnd10DetectClsnEv(raycast))
        mGroundY = *(int *)(raycast + 0x44);
    _ZN9dBgCh_GndD1Ev(raycast);

    return 1;
}
