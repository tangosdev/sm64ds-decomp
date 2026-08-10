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
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *fp);
}
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern "C" {
extern void func_020393d4(int *p, int v);
extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, void *pos, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void _ZN13RaycastGroundD1Ev(void *self);
}
extern int _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;


struct Vec3 { int x, y, z; };

int TTC_MovingBar::InitResources()
{
    struct Vec3 pos;
    char raycast[0x50];
    int i;

    if (actorID != 0x72) {
        if (actorID == 0x73)
            unk_31e = 1;
    } else {
        unk_31e = 0;
    }

    i = unk_31e;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(
        &mModel,
        _ZN5Model8LoadFileER13SharedFilePtr(*(void **)(data_ov065_0211d35c + i * 0xc)),
        1, -1);

    _ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel);
    _ZN8Platform21UpdateModelPosAndRotYEv(((char *)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));

    i = unk_31e;
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(void **)(data_ov065_0211d360 + i * 0xc)),
        &mClsnMat,
        0x199,
        mAngleY,
        *(void **)(data_ov065_0211d364 + i * 0xc));

    func_020393d4((int *)((char *)&(*(u8 *)&mMeshCollider)), (int)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y = pos.y - 0xa000;

    _ZN13RaycastGroundC1Ev(raycast);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(raycast, &pos, (void *)0);
    unk_320 = pos.y;
    if (_ZN13RaycastGround10DetectClsnEv(raycast))
        unk_320 = *(int *)(raycast + 0x44);
    _ZN13RaycastGroundD1Ev(raycast);

    return 1;
}
