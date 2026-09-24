//cpp
/* daObjCvNewsLift_c: the four-platform lift (CV_NEWS_LIFT). A dBgActor_c
 * whose main mesh carries four hanging platforms, each with its own model
 * and moving-mesh collider. Stepping on a platform lowers it; standing on
 * the main mesh tilts the lift toward the player.
 *
 * mwccarm emits .text in reverse source order, so the highest ROM address
 * (the factory) is written first and the destructor last. */

#include "daObjCvNewsLift_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Gnd.h"
#include "Sound.h"

struct Quaternion;

/* A SharedFilePtr seen from outside: SharedFilePtr.h deliberately declares
 * no fields, and these reads need the loaded buffer at +4. */
struct NewsLiftFile {
    u32 unk_00;
    void *file;
};

extern NewsLiftFile data_ov021_021149a0;
extern NewsLiftFile data_ov021_021149a8;
extern NewsLiftFile data_ov021_021149b0;
extern NewsLiftFile data_ov021_021149b8;
extern CLPS_Block data_ov021_02113a60;
extern CLPS_Block data_ov021_02113a80;

extern "C" {
void Matrix4x3_FromQuaternion(const struct Quaternion *q, struct Matrix4x3 *mF);
void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
void MulMat4x3Mat4x3(void *a, void *b, void *c);
void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *mF, s16 angX);
void Matrix4x3_ApplyInPlaceToRotationZ(struct Matrix4x3 *mF, s16 angZ);
void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *mF, s16 angY);
void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *mF, int x, int y, int z);
void Vec3_Asr(void *d, void *s, int sh);
void InvMat4x3(struct Matrix4x3 *d, struct Matrix4x3 *s);
void MulVec3Mat4x3(const Vector3 *src, const Matrix4x3 *mtx, Vector3 *dst);
void AddVec3(const Vector3 *a, const Vector3 *b, Vector3 *dst);
void Matrix4x3_FromRotationY(Matrix4x3 *m, s16 angle);
void Quaternion_FromVector3(void *q, Vector3 *axis, Vector3 *v);
void Quaternion_Normalize(void *q);
void Quaternion_SLerp(void *q0, void *q1, int t, void *out);
s32 Vec3_Equal(const Vector3 *a, const Vector3 *b);
u16 DecIfAbove0_Short(u16 *p);
u8 DecIfAbove0_Byte(u8 *p);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    Fix12i scale, s16 angle, CLPS_Block *clps);
void func_020393d4(dBgW *collider, void *callback);
void func_020393c4(dBgW *collider, void *callback);

extern struct Matrix4x3 data_020a0e68;
extern s32 data_02092768[];
extern s16 data_02082214[];
extern const Vector3 data_ov021_02114a20[4];
extern const u16 data_ov021_02114740[];

void func_ov021_0211129c(daObjCvNewsLift_c *self);
void func_ov021_02111434(daObjCvNewsLift_c *self);
void func_ov021_02111ec4(daObjCvNewsLift_c *self, dActor_c *other);
void func_ov021_02111f1c(daObjCvNewsLift_c *self, dActor_c *other);
void func_ov021_02111f74(daObjCvNewsLift_c *self, dActor_c *other);
void func_ov021_02111fcc(daObjCvNewsLift_c *self, dActor_c *other);
void func_ov021_02112024(daObjCvNewsLift_c *self, dActor_c *other);
}

// @symbol daObjCvNewsLift_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjCvNewsLift_c through
 * RTTI, allocation size, vtable identity, and the CV_NEWS_LIFT registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical aliases: WorkElevator (the class) and
 * WorkElevator_Spawn. */
extern "C" daObjCvNewsLift_c *daObjCvNewsLift_c_classInit()
{
    return new daObjCvNewsLift_c();
}

// @symbol func_ov021_02112128
/* Collision callbacks. InitResources registers one per collider; each fires
 * when an actor touches that mesh and reacts only to the player (0xbf). The
 * comparison is kept in an int: testing it inline drops the ROM's moveq/movne
 * materialisation. */
extern "C" void func_ov021_02112128(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other)
{
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        func_ov021_02112024(self, other);
}

// @symbol func_ov021_02112024
/* The player is on the main mesh: hold the lift and, unless it is bumped,
 * tilt the target pose toward the player's position in the lift's frame. */
extern "C" void func_ov021_02112024(daObjCvNewsLift_c *self, dActor_c *other)
{
    self->mPlayerOnMesh = 1;
    self->mResetTimer = 300;
    if (self->mBumped) {
        self->mTargetRotation[0] = data_02092768[0];
        self->mTargetRotation[1] = data_02092768[1];
        self->mTargetRotation[2] = data_02092768[2];
        self->mTargetRotation[3] = data_02092768[3];
    } else {
        data_020a0e68 = self->mClsnMat;
        InvMat4x3(&data_020a0e68, &data_020a0e68);
        Vector3 t;
        MulVec3Mat4x3((Vector3 *)&other->mPosX, &data_020a0e68, &t);
        t.y = t.y * 0x30;
        Vector3 axis;
        axis.x = 0;
        axis.y = 0x1000;
        axis.z = 0;
        Quaternion_FromVector3(&self->mTargetRotation[0], &axis, &t);
        Quaternion_Normalize(&self->mTargetRotation[0]);
        self->mTiltHoldTimer = 10;
    }
}

// @symbol func_ov021_02111fe4
extern "C" void func_ov021_02111fe4(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other)
{
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        func_ov021_02111fcc(self, other);
}

// @symbol func_ov021_02111fcc
extern "C" void func_ov021_02111fcc(daObjCvNewsLift_c *self, dActor_c *other)
{
    self->mResetTimer = 300;
    self->mLoweredPlatform = 0;
}

// @symbol func_ov021_02111f8c
extern "C" void func_ov021_02111f8c(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other)
{
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        func_ov021_02111f74(self, other);
}

// @symbol func_ov021_02111f74
extern "C" void func_ov021_02111f74(daObjCvNewsLift_c *self, dActor_c *other)
{
    self->mResetTimer = 300;
    self->mLoweredPlatform = 1;
}

// @symbol func_ov021_02111f34
extern "C" void func_ov021_02111f34(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other)
{
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        func_ov021_02111f1c(self, other);
}

// @symbol func_ov021_02111f1c
extern "C" void func_ov021_02111f1c(daObjCvNewsLift_c *self, dActor_c *other)
{
    self->mResetTimer = 300;
    self->mLoweredPlatform = 2;
}

// @symbol func_ov021_02111edc
extern "C" void func_ov021_02111edc(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other)
{
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        func_ov021_02111ec4(self, other);
}

// @symbol func_ov021_02111ec4
extern "C" void func_ov021_02111ec4(daObjCvNewsLift_c *self, dActor_c *other)
{
    self->mResetTimer = 300;
    self->mLoweredPlatform = 3;
}

// @symbol _ZN17daObjCvNewsLift_c13InitResourcesEv
s32 daObjCvNewsLift_c::InitResources()
{
    Model::LoadFile(*(SharedFilePtr *)&data_ov021_021149a0);
    dBgW_Kc::LoadFile(*(SharedFilePtr *)&data_ov021_021149a8);
    Model::LoadFile(*(SharedFilePtr *)&data_ov021_021149b0);
    dBgW_Kc::LoadFile(*(SharedFilePtr *)&data_ov021_021149b8);

    mModel.SetFile((BMD_File *)data_ov021_021149a0.file, 1, -1);
    {
        int i = 0;
        Model *platformModel = mPlatformModels;
        for (; i < 4; i++, platformModel++)
            platformModel->SetFile((BMD_File *)data_ov021_021149b0.file, 1, -1);
    }

    mLoweredPlatform = -1;
    func_ov021_02111434(this);
    func_ov021_0211129c(this);

    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)data_ov021_021149a8.file, &mClsnMat,
        0x199, mAngleY, &data_ov021_02113a60);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    func_020393c4(&mMeshCollider, (void *)&func_ov021_02112128);
    mMeshCollider.Enable(this);

    {
        int i = 0;
        Matrix4x3 *platformMat = mPlatformMats;
        dBgW_KcMbg *platformCollider = mPlatformColliders;
        for (; i < 4; i++, platformMat++, platformCollider++) {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                platformCollider, (KCL_File *)data_ov021_021149b8.file, platformMat,
                0x199, mAngleY, &data_ov021_02113a80);
            func_020393d4(platformCollider, (void *)&dBgW::UpdatePosWithTransform);
        }
    }

    func_020393c4(&mPlatformColliders[0], (void *)&func_ov021_02111fe4);
    func_020393c4(&mPlatformColliders[1], (void *)&func_ov021_02111f8c);
    func_020393c4(&mPlatformColliders[2], (void *)&func_ov021_02111f34);
    func_020393c4(&mPlatformColliders[3], (void *)&func_ov021_02111edc);

    for (int i = 0; i < 4; i++)
        mPlatformColliders[i].Enable(this);

    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;

    Vector3 probePos;
    probePos.x = mPosX;
    probePos.y = mPosY;
    probePos.z = mPosZ;
    probePos.y -= 0x14000;

    dBgCh_Gnd ground;
    ground.SetObjAndPos(probePos, 0);
    mGroundY = probePos.y;
    if (ground.DetectClsn())
        mGroundY = ground.clsnY;
    return 1;
}

// @symbol _ZN17daObjCvNewsLift_c8BehaviorEv
/* While the player stands on a platform the lift drives along that
 * platform's facing and casts three lines ahead; a hit within 0x1f000 bumps
 * it. A bump wobbles the lift from the sine table, then reverses the
 * direction (0 and 1, 2 and 3 are opposite platforms). The shared tail slerps
 * the tilt toward its target, snaps home when mResetTimer runs out and
 * rebuilds both sets of matrices.
 * Measured: the double cast on mWobblePhase, the volatile reads in the
 * reset and the per-element pointer writes to va[1..2] are load-bearing;
 * dropping any one rewrites the function. */
s32 daObjCvNewsLift_c::Behavior()
{
    Vector3 va[3];
    Vector3 vb[3];
    Vector3 vc[3];
    Vector3 sp74;
    s32 i;

    if (mBumped != 0) {
        if (DecIfAbove0_Short(&mWobbleTimer) != 0) {
            mWobblePhase += 0x1f00;
            {
                s16 v = data_02082214[((u16)(s16)mWobblePhase >> 4) * 2];
                s32 prod = mWobbleTimer * 0x1a;
                s8 sel = mLoweredPlatform;
                s16 t = (s16)(s32)(((long long)prod * v + 0x800) >> 12);
                switch (sel) {
                case 0: mAngleX = -t; break;
                case 1: mAngleX = t; break;
                case 2: mAngleZ = t; break;
                case 3: mAngleZ = -t; break;
                }
            }
        } else {
            s8 s = mLoweredPlatform;
            if (s == 0 || s == 2)
                mLoweredPlatform++;
            else
                mLoweredPlatform--;
            mBumped = 0;
            mPrevAngleY = data_ov021_02114740[mLoweredPlatform];
            mAngleX = 0;
            mAngleZ = 0;
            mPosY = mHomePos.y;
        }
        if (mPrevLoweredPlatform != mLoweredPlatform) {
            mBumped = 0;
            mPrevAngleY = data_ov021_02114740[mLoweredPlatform];
            mAngleX = 0;
            mAngleZ = 0;
            mPosY = mHomePos.y;
        }
    } else {
        if (mLoweredPlatform != -1) {
            mMotorSound = Sound::PlayLong(mMotorSound, 3, 0x88, *(Vector3 *)&mCamSpacePosX, 0);
            mPrevAngleY = data_ov021_02114740[mLoweredPlatform];
            mHorzSpeed = 0x5000;
            va[0].x = 0; va[0].y = -0xa000; va[0].z = 0x10e000;
            {
                Vector3 *p = &va[1];
                p->x = 0x12c000; p->y = -0xa000; p->z = 0x10e000;
                p = &va[2];
                p->x = -0x12c000; p->y = -0xa000; p->z = 0x10e000;
            }
            vb[0].x = 0; vb[0].y = 0; vb[0].z = 0;
            {
                Vector3 *p = &vb[1];
                p->x = 0; p->y = 0; p->z = 0;
                p = &vb[2];
                p->x = 0; p->y = 0; p->z = 0;
            }
            vc[0].x = 0; vc[0].y = 0; vc[0].z = 0;
            {
                Vector3 *p = &vc[1];
                p->x = 0; p->y = 0; p->z = 0;
                p = &vc[2];
                p->x = 0; p->y = 0; p->z = 0;
            }
            Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
            {
                Vector3 *pa = va;
                Vector3 *pb = vb;
                Vector3 *pc = vc;
                for (i = 0; i < 3; i++) {
                    sp74 = *pa;
                    sp74.z += 0x28000;
                    MulVec3Mat4x3(pa, &data_020a0e68, pb);
                    MulVec3Mat4x3(&sp74, &data_020a0e68, pc);
                    AddVec3(pb, (Vector3 *)&mPosX, pb);
                    AddVec3(pc, (Vector3 *)&mPosX, pc);
                    pa++; pb++; pc++;
                }
            }
            {
                dBgCh_Lin l0;
                dBgCh_Lin l1;
                dBgCh_Lin l2;
                {
                    s32 j = 0;
                    Vector3 *pc = vc;
                    Vector3 *pb = vb;
                    dBgCh_Lin *pl = &l0;
                    u32 snd = 0x1b;
                    for (; j < 3; j++) {
                        pl->SetObjAndLine(*pb, *pc, this);
                        if (pl->DetectClsn() && pl->clsnDist <= 0x1f000) {
                            mHorzSpeed = 0;
                            mBumped = 1;
                            mWobbleTimer = 0x3c;
                            Sound::PlayBank3(snd, *(Vector3 *)&mCamSpacePosX);
                        }
                        pc++; pb++; pl++;
                    }
                }
                UpdatePos(0);
            }
        }
        if (DecIfAbove0_Byte(&mTiltHoldTimer) == 0) {
            mTargetRotation[0] = data_02092768[0];
            mTargetRotation[1] = data_02092768[1];
            mTargetRotation[2] = data_02092768[2];
            mTargetRotation[3] = data_02092768[3];
        }
    }
    Quaternion_SLerp(&mRotation[0], &mTargetRotation[0], 0x199, &mRotation[0]);
    if (Vec3_Equal((Vector3 *)&mPosX, &mHomePos) != 0)
        mResetTimer = 0x12c;
    if (DecIfAbove0_Short(&mResetTimer) == 0) {
        mPosX = mHomePos.x;
        mPosY = mHomePos.y;
        mPosZ = mHomePos.z;
        mRotation[0] = *(volatile s32 *)&data_02092768[0];
        mRotation[1] = *(volatile s32 *)&data_02092768[1];
        mRotation[2] = *(volatile s32 *)&data_02092768[2];
        mRotation[3] = *(volatile s32 *)&data_02092768[3];
        mTargetRotation[0] = *(volatile s32 *)&data_02092768[0];
        mTargetRotation[1] = *(volatile s32 *)&data_02092768[1];
        mTargetRotation[2] = *(volatile s32 *)&data_02092768[2];
        mTargetRotation[3] = *(volatile s32 *)&data_02092768[3];
        mLoweredPlatform = -1;
        mWobblePhase = 0;
        mBumped = 0;
        mWobbleTimer = 0;
        mAngleX = 0;
        mAngleZ = 0;
        mHorzSpeed = 0;
    }
    if (mBumped == 0 && mResetTimer < 0x10e) {
        mLoweredPlatform = -1;
        mHorzSpeed = 0;
    }
    func_ov021_02111434(this);
    func_ov021_0211129c(this);
    mPlayerOnMesh = 0;
    mHorzSpeed = 0;
    if (mPrevLoweredPlatform != mLoweredPlatform)
        Sound::PlayBank3(0x3e, *(Vector3 *)&mCamSpacePosX);
    mPrevLoweredPlatform = mLoweredPlatform;
    return 1;
}

// @symbol _ZN17daObjCvNewsLift_c6RenderEv
/* Blinks on odd frames while the reset countdown is under 45. */
s32 daObjCvNewsLift_c::Render()
{
    if (mResetTimer < 45 && (mResetTimer & 1))
        return 1;
    mModel.Render(0);
    for (int i = 0; i < 4; i++)
        mPlatformModels[i].Render(0);
    return 1;
}

// @symbol _ZN17daObjCvNewsLift_c16CleanupResourcesEv
s32 daObjCvNewsLift_c::CleanupResources()
{
    int i;
    mMeshCollider.Disable();
    dBgW_KcMbg *platformCollider = mPlatformColliders;
    for (i = 0; i < 4; i++) {
        platformCollider->Disable();
        platformCollider++;
    }
    ((SharedFilePtr *)&data_ov021_021149b0)->Release();
    ((SharedFilePtr *)&data_ov021_021149b8)->Release();
    ((SharedFilePtr *)&data_ov021_021149a0)->Release();
    ((SharedFilePtr *)&data_ov021_021149a8)->Release();
    return 1;
}

// @symbol func_ov021_02111434
/* The render-side twin of func_ov021_0211129c: the same pose, built at model
 * scale (positions shifted down by 3), written into the models' matrices. */
extern "C" void func_ov021_02111434(daObjCvNewsLift_c *self)
{
    Matrix4x3 tilt;
    int i;
    Matrix4x3 *mainMat;
    const Vector3 *offset;
    Vector3 v;
    Vector3 pos;
    Vector3 scaled;

    Matrix4x3_FromQuaternion((Quaternion *)self->mRotation, &tilt);
    Vec3_Asr(&pos, &self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos.x, pos.y, pos.z);
    MulMat4x3Mat4x3(&tilt, &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, self->mAngleX);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, self->mAngleZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, self->mAngleY);
    self->mModel.mat4x3 = data_020a0e68;
    mainMat = &self->mModel.mat4x3;
    offset = data_ov021_02114a20;
    i = 0;
    for (; i < 4; i++) {
        v = *offset;
        if (self->mLoweredPlatform == i && !self->mBumped)
            v.y -= 0x1e000;
        data_020a0e68 = *mainMat;
        Vec3_Asr(&scaled, &v, 3);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, scaled.x, scaled.y, scaled.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, data_ov021_02114740[i]);
        self->mPlatformModels[i].mat4x3 = data_020a0e68;
        offset++;
    }

    unsigned timer = self->mResetTimer;
    /* Empty early returns: the ROM keeps the dead flag-setting test. */
    if (timer >= 0x2d)
        return;
    if (timer & 1)
        return;
}

// @symbol func_ov021_0211129c
/* Rebuild the collision matrices: the main mesh from position, tilt and
 * facing, then each platform hung at its offset (lowered by 0x1e000 while
 * the player stands on it and the lift is not bumped). */
extern "C" void func_ov021_0211129c(daObjCvNewsLift_c *self)
{
    Matrix4x3 tilt;
    int i;
    const Vector3 *offset;
    daObjCvNewsLift_c *platform;
    Matrix4x3 *platformMat;
    dBgW_KcMbg *platformCollider;
    Vector3 v;

    Matrix4x3_FromQuaternion((Quaternion *)self->mRotation, &tilt);
    Matrix4x3_FromTranslation(&data_020a0e68, self->mPosX, self->mPosY, self->mPosZ);
    MulMat4x3Mat4x3(&tilt, &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, self->mAngleX);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, self->mAngleZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, self->mAngleY);
    self->mClsnMat = data_020a0e68;
    self->mMeshCollider.Transform(self->mClsnMat, self->mAngleY);

    offset = data_ov021_02114a20;
    i = 0;
    platform = self;
    platformMat = self->mPlatformMats;
    platformCollider = self->mPlatformColliders;
    for (; i < 4; i++) {
        v = *offset;
        if (self->mLoweredPlatform == i && !self->mBumped)
            v.y -= 0x1e000;
        data_020a0e68 = self->mClsnMat;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, v.x, v.y, v.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, data_ov021_02114740[i]);
        /* The ROM stores through a 0x30-stepped object pointer with the
         * +0x460 folded into the str. mPlatformMats[i] misses by 8 words;
         * a second matrix pointer reallocates the whole loop. */
        *(Matrix4x3 *)((char *)platform + 0x460) = data_020a0e68;
        platformCollider->Transform(*platformMat, self->mAngleY);
        offset++;
        platform = (daObjCvNewsLift_c *)((char *)platform + 0x30);
        platformMat++;
        platformCollider++;
    }
}
