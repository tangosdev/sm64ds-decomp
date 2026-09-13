//cpp
/* ov100/daObjPathLift_c -- PATH_LIFT (31).
 *
 * SM64DS proves daObjPathLift_c through RTTI (_ZTI/_ZTS at ov100
 * 0x02148538/0x02148544), the PATH_LIFT registry profile, and the factory's
 * allocation/vptr shape. ov100 is mixed; this is the path lift, not STAR_DOOR
 * / daStarGate_c. Historical project alias: PathLift.
 *
 * mwccarm emits ordinary function sections in reverse source order. Keep
 * InitResources first. The inline destructor declared in daObjPathLift_c
 * emits the retail D1/D0 pair first and emits no leaf D2 body.
 *
 * The factory stays in src/d_a_obj_path_lift.cpp. Folding `return new`
 * into this TU parks the vague-linkage Vector3 D1 between InitResources
 * and classInit and shifts the factory.
 */

#include "daObjPathLift_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"

extern "C" {
extern CLPS_Block data_ov002_0210d7d4;
extern void func_ov002_020efcf4(dPathLiftActor_c *lift);
extern void func_ov002_020efc74(dPathLiftActor_c *lift);
extern void func_ov002_020efaf0(dPathLiftActor_c *lift);
extern void func_ov100_02146e70(daObjPathLift_c *self);
extern int func_ov100_0214700c(daObjPathLift_c *self);

extern void Matrix4x3_FromRotationY(Matrix4x3 *m, short ang);
extern void Matrix4x3_FromRotationXYZExt(Matrix4x3 *dst, int a, int b, int c2);
extern short data_02082214[];
extern signed char data_0209f2f8;
extern unsigned char data_0209f2d8;
extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void func_020393a4(dBgW_KcMbg *p, int v);
extern void func_02039394(dBgW_KcMbg *p, int v);
extern void func_020393d4(dBgW_KcMbg *p, void *v);

/* By-value Fix12<int> is wall 6az: constructing wrapper arguments adds stack
   traffic the ROM does not have. Keep the register-level spelling. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, void *kcl, const Matrix4x3 *mtx, int scale, short angY,
    CLPS_Block *clps);
void _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(
    dBgActor_c *self, int a, int b);
void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    dActor_c *self, ShadowModel *sm, Matrix4x3 *mtx, int a, int b, int d,
    unsigned int e);
}

/* The fifth parameter is a signed short, not an unsigned int. Declared
   `unsigned int` this mangles _ZN5Sound8PlayLongEjjjRK7Vector3j. */
namespace Sound {
unsigned int PlayLong(unsigned int, unsigned int, unsigned int,
                      const Vector3 &, short);
}

extern SharedFilePtr data_ov002_0210d9f0;
extern SharedFilePtr data_ov100_02148a54;
extern SharedFilePtr data_ov100_02148a5c;

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjPathLift_c13InitResourcesEv
int daObjPathLift_c::InitResources()
{
    Vector3 pos;
    Model::LoadFile(data_ov002_0210d9f0);
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov100_02148a54), 1, -1);
    mShadowModel.InitCuboid();
    func_ov100_0214700c(this);
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, dBgW_Kc::LoadFile(data_ov100_02148a5c),
        &mClsnMat, 0x1000, mAngleY, &data_ov002_0210d7d4);
    func_020393d4(
        &mMeshCollider,
        (void *)&dBgW::UpdatePosAndAngs);
    mPathSpeed = 0xa000;
    mHorzSpeed = mPathSpeed;
    func_ov002_020efaf0(this);
    mPathDirection = 1;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y -= 0x14000;
    {
        dBgCh_Gnd rg;
        int b;
        rg.SetObjAndPos(pos, 0);
        mGroundY = pos.y;
        if (rg.DetectClsn() != 0)
            mGroundY = rg.clsnY;
        unk_42c = 1;
        b = (data_0209f2d8 == 1);
        if (b)
            mTimer = 0xb4;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjPathLift_c8BehaviorEv
int daObjPathLift_c::Behavior()
{
    func_ov002_020efcf4(this);
    BaseBehavior();
    if (Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&mPrevPosX) != 0) {
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            mSoundHandle = Sound::PlayLong(
                mSoundHandle, 3, 0x82, *(Vector3 *)&mCamSpacePosX, 0);
        }
    }
    func_ov100_0214700c(this);
    UpdateClsnPosAndRot();
    func_ov100_02146e70(this);
    func_020393a4(&mMeshCollider, 0x150000);
    func_02039394(&mMeshCollider, 0x1000);
    int b = (int)(data_0209f2d8 == 1);
    if (b != 0) {
        if (mMeshCollider.IsEnabled() == 0) {
            mMeshCollider.Enable(this);
        }
    } else {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjPathLift_c6RenderEv
int daObjPathLift_c::Render()
{
    unsigned short h = mWaitTimer;
    if (h < 0x5a) {
        if (h & 1)
            return 1;
    }
    func_ov002_020efc74(this);
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjPathLift_c16CleanupResourcesEv
int daObjPathLift_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov002_0210d9f0.Release();
    data_ov100_02148a54.Release();
    data_ov100_02148a5c.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_0214700c
extern "C" int func_ov100_0214700c(daObjPathLift_c *self)
{
    Matrix4x3_FromRotationXYZExt(
        &self->mModel.mat4x3, self->mAngleX, self->mAngleY, 0);
    /* Direct nested-member stores make b56 materialise a second matrix base
       and grow this function. Retail keeps `this` as the base for the three
       translation stores. */
    *(s32 *)((char *)self + 0x114) = self->mPosX >> 3;
    *(s32 *)((char *)self + 0x118) = self->mPosY >> 3;
    int z = self->mPosZ >> 3;
    *(s32 *)((char *)self + 0x11c) = z;
    return z;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_02146e70
extern "C" void func_ov100_02146e70(daObjPathLift_c *self)
{
    int scaleZ;
    int yOff;
    int groundDepth;
    int idx;
    int sinVal;
    int cosVal;
    int t;

    if (data_0209f2f8 == 13)
        return;
    if (self->mGroundY > self->mPosY)
        return;

    groundDepth = (self->mPosY - self->mGroundY) + 0x64000;

    idx = (unsigned short)self->mAngleX >> 4;
    sinVal = data_02082214[idx << 1];
    yOff = (int)(((long long)sinVal * 0x17c000 + 0x800) >> 12);
    if (yOff < 0)
        yOff = -yOff;
    cosVal = data_02082214[(idx << 1) + 1];
    scaleZ = (int)(((long long)cosVal * 0x17c000 + 0x800) >> 12);
    if (scaleZ < 0)
        scaleZ = -scaleZ;

    Matrix4x3_FromRotationY((Matrix4x3 *)self->unk_478, self->mAngleY);
    *(s32 *)((char *)self + 0x49c) = self->mPosX >> 3;
    *(s32 *)((char *)self + 0x4a0) = (self->mPosY - yOff) >> 3;
    *(s32 *)((char *)self + 0x4a4) = self->mPosZ >> 3;

    t = data_0209f2d8;
    t = t == 1;
    if (t != false) {
        _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            self, &self->mShadowModel, (Matrix4x3 *)self->unk_478, 0x118000,
            0x7d0000, scaleZ, 0xf);
    } else {
        _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            self, &self->mShadowModel, (Matrix4x3 *)self->unk_478, 0x118000,
            groundDepth, scaleZ, 0xf);
    }
}
