//cpp
// @symbol _ZN15daObjPathLift_c13InitResourcesEv
/* daObjPathLift_c::InitResources -- vtable slot 2. The resource/model/collider
 * setup now uses the recovered class hierarchy and named subobjects. The three
 * raw offsets are inherited PathLift tail state; see daObjPathLift_c.h. */
#include "daObjPathLift_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"

extern "C" {
/* dBgW_KcMbg::SetFile carries a by-value Fix12<int>. Its faithful member call
 * triggers mwccarm's stack-homing ABI and does not reproduce this call site, so
 * keep the verified scalar shim (runbook wall 6az). */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *file, void *mat, int scale, short angY, void *clps);
void func_020393d4(void *collider, void *callback);
void func_ov002_020efaf0(daObjPathLift_c *self);
int func_ov100_0214700c(daObjPathLift_c *self);

/* dBgCh_Gnd is a real class now (notes/ctor-migration.md item 9): `ground`
 * below is constructed at its declaration and destroyed at the closing brace,
 * both calls synthesized. */
int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd *self);

extern SharedFilePtr data_ov002_0210d9f0;
extern SharedFilePtr data_ov100_02148a54;
extern SharedFilePtr data_ov100_02148a5c;
extern int data_ov002_0210d7d4;
extern void _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
}
extern unsigned char data_0209f2d8;

int daObjPathLift_c::InitResources()
{
    Vector3 pos;
    Model::LoadFile(data_ov002_0210d9f0);
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov100_02148a54), 1, -1);
    mShadowModel.InitCuboid();
    func_ov100_0214700c(this);
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, dBgW_Kc::LoadFile(data_ov100_02148a5c), &mClsnMat,
        0x1000, mAngleY, &data_ov002_0210d7d4);
    func_020393d4(
        &mMeshCollider,
        (void *)_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    *(s32 *)((char *)this + 0x440) = 0xa000;
    mHorzSpeed = *(s32 *)((char *)this + 0x440);
    func_ov002_020efaf0(this);
    *(s32 *)((char *)this + 0x43c) = 1;

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y -= 0x14000;
    {
        dBgCh_Gnd ground;
        int debugMode;
        ground.SetObjAndPos(pos, 0);
        mGroundY = pos.y;
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&ground) != 0)
            mGroundY = ground.clsnY;
        *(u8 *)((char *)this + 0x42c) = 1;
        debugMode = (data_0209f2d8 == 1);
        if (debugMode)
            mTimer = 0xb4;
        /* ground's destructor is synthesized at this brace */
    }
    return 1;
}
