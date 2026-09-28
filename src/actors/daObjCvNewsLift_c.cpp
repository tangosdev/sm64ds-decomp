//cpp
/* ov021/daObjCvNewsLift_c -- the four-platform lift (CV_NEWS_LIFT).
 *
 * A dBgActor_c. The main mesh carries four hanging platforms, each with
 * its own model and moving-mesh collider. Stepping on a platform lowers
 * it and drives the lift that way; standing on the main mesh tilts the
 * lift toward the player. A wall ahead bumps it, wobbles, then reverses
 * onto the opposite platform (0/1 and 2/3). Left alone it blinks and
 * snaps home.
 *
 * mwccarm emits .text in reverse source order, so the factory is written
 * first and the destructor last.
 *
 * deslop leftovers:
 * - InitResources: dBgW_KcMbg::SetFile as a method is 0x264 against 0x258
 *   (Fix12<int> by value). The scalar mangled call stays.
 * - InitResources: mMeshCollider.unk_1c = &MainMeshCallback, and a cast
 *   store of UpdatePosWithTransform into beforeClsnCallback, are each
 *   0x254 against 0x258. func_020393c4 and func_020393d4 stay.
 * - InitResources: probePos.y = mPosY - 0x14000 is 0x254 against 0x258.
 * - InitResources: Model::LoadFile's return in place of filePtr differs
 *   by 20 words. NewsLiftFile stays; SharedFilePtr has no fields.
 * - OnMainMeshRide: copying mTargetRotation in a loop is 0x100 against
 *   0x104.
 * - Behavior: reset reads of data_02092768 without volatile are 0x594
 *   against 0x5a4. Dropping (u16)(s16) on mWobblePhase is 0x59c, and so
 *   is writing rayStart[1] and rayStart[2] by name. Dropping the zero
 *   fills of rayFrom and rayTo is 0x54c.
 * - UpdateClsnTransforms: mPlatformMats[i] = differs by 8 words;
 *   *platformMat = is 0x194 against 0x198; folding i = 0 into the
 *   for-init differs by 4 words. The this-cursor store stays.
 * - UpdateModelTransforms: an empty blink test is 0x18c against 0x1a8.
 *   The trailing return stays.
 * - MainMeshCallback and Platform0Callback: testing actorID == 0xbf
 *   inline is 0x34 against 0x40. The int temporary stays on all five
 *   callbacks.
 * - data_ov021_* handles, data_02092768, data_020a0e68 and data_02082214
 *   stay address names (__sinit_ov021_02113500 owns the handles).
 *   dActor_c has no Pos(), so callers cast &mPosX.
 */

#include "daObjCvNewsLift_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Gnd.h"
#include "Sound.h"

struct Quaternion;

/* A SharedFilePtr seen from outside: SharedFilePtr.h deliberately declares
 * no fields, and these reads need the loaded buffer. The fields follow the
 * layout that _ZN13SharedFilePtr4LoadEv's definition uses. */
struct NewsLiftFile {
    u16 fileID;
    u8 numRefs;
    void *filePtr;
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
/* Scalar SetFile. The method form size-DIFFs InitResources; see leftovers. */
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
}

// @symbol daObjCvNewsLift_c_classInit
/* CV_NEWS_LIFT factory. classInit is the EAD shape; the cartridge's RTTI
 * names the class, not this function. */
extern "C" daObjCvNewsLift_c *daObjCvNewsLift_c_classInit()
{
    return new daObjCvNewsLift_c();
}

// @symbol _ZN17daObjCvNewsLift_c16MainMeshCallbackEP4dBgWPS_P8dActor_c
/* One callback per collider. Only the player (actor 0xbf) is handled.
 * The comparison stays in an int; see leftovers. */
void daObjCvNewsLift_c::MainMeshCallback(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other)
{
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        self->OnMainMeshRide(other);
}

// @symbol _ZN17daObjCvNewsLift_c14OnMainMeshRideEP8dActor_c
/* The player is on the main mesh: hold the lift and, unless it is bumped,
 * tilt the target pose toward the player's position in the lift's frame. */
void daObjCvNewsLift_c::OnMainMeshRide(dActor_c *player)
{
    mPlayerOnMesh = 1;
    mResetTimer = 300;
    if (mBumped) {
        mTargetRotation[0] = data_02092768[0];
        mTargetRotation[1] = data_02092768[1];
        mTargetRotation[2] = data_02092768[2];
        mTargetRotation[3] = data_02092768[3];
    } else {
        data_020a0e68 = mClsnMat;
        InvMat4x3(&data_020a0e68, &data_020a0e68);
        Vector3 inFrame;
        MulVec3Mat4x3((Vector3 *)&player->mPosX, &data_020a0e68, &inFrame);
        inFrame.y *= 0x30;
        Vector3 axis;
        axis.x = 0;
        axis.y = 0x1000;
        axis.z = 0;
        Quaternion_FromVector3(&mTargetRotation[0], &axis, &inFrame);
        Quaternion_Normalize(&mTargetRotation[0]);
        mTiltHoldTimer = 10;
    }
}

// @symbol _ZN17daObjCvNewsLift_c17Platform0CallbackEP4dBgWPS_P8dActor_c
void daObjCvNewsLift_c::Platform0Callback(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other)
{
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        self->OnPlatform0Ride(other);
}

// @symbol _ZN17daObjCvNewsLift_c15OnPlatform0RideEP8dActor_c
void daObjCvNewsLift_c::OnPlatform0Ride(dActor_c *player)
{
    mResetTimer = 300;
    mLoweredPlatform = 0;
}

// @symbol _ZN17daObjCvNewsLift_c17Platform1CallbackEP4dBgWPS_P8dActor_c
void daObjCvNewsLift_c::Platform1Callback(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other)
{
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        self->OnPlatform1Ride(other);
}

// @symbol _ZN17daObjCvNewsLift_c15OnPlatform1RideEP8dActor_c
void daObjCvNewsLift_c::OnPlatform1Ride(dActor_c *player)
{
    mResetTimer = 300;
    mLoweredPlatform = 1;
}

// @symbol _ZN17daObjCvNewsLift_c17Platform2CallbackEP4dBgWPS_P8dActor_c
void daObjCvNewsLift_c::Platform2Callback(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other)
{
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        self->OnPlatform2Ride(other);
}

// @symbol _ZN17daObjCvNewsLift_c15OnPlatform2RideEP8dActor_c
void daObjCvNewsLift_c::OnPlatform2Ride(dActor_c *player)
{
    mResetTimer = 300;
    mLoweredPlatform = 2;
}

// @symbol _ZN17daObjCvNewsLift_c17Platform3CallbackEP4dBgWPS_P8dActor_c
void daObjCvNewsLift_c::Platform3Callback(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other)
{
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        self->OnPlatform3Ride(other);
}

// @symbol _ZN17daObjCvNewsLift_c15OnPlatform3RideEP8dActor_c
void daObjCvNewsLift_c::OnPlatform3Ride(dActor_c *player)
{
    mResetTimer = 300;
    mLoweredPlatform = 3;
}

// @symbol _ZN17daObjCvNewsLift_c13InitResourcesEv
s32 daObjCvNewsLift_c::InitResources()
{
    Model::LoadFile(*(SharedFilePtr *)&data_ov021_021149a0);
    dBgW_Kc::LoadFile(*(SharedFilePtr *)&data_ov021_021149a8);
    Model::LoadFile(*(SharedFilePtr *)&data_ov021_021149b0);
    dBgW_Kc::LoadFile(*(SharedFilePtr *)&data_ov021_021149b8);

    mModel.SetFile((BMD_File *)data_ov021_021149a0.filePtr, 1, -1);
    for (int i = 0; i < 4; i++)
        mPlatformModels[i].SetFile((BMD_File *)data_ov021_021149b0.filePtr, 1, -1);

    mLoweredPlatform = -1;
    UpdateModelTransforms();
    UpdateClsnTransforms();

    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)data_ov021_021149a8.filePtr, &mClsnMat,
        0x199, mAngleY, &data_ov021_02113a60);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    func_020393c4(&mMeshCollider, (void *)&MainMeshCallback);
    mMeshCollider.Enable(this);

    for (int i = 0; i < 4; i++) {
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mPlatformColliders[i], (KCL_File *)data_ov021_021149b8.filePtr,
            &mPlatformMats[i], 0x199, mAngleY, &data_ov021_02113a80);
        func_020393d4(&mPlatformColliders[i], (void *)&dBgW::UpdatePosWithTransform);
    }

    func_020393c4(&mPlatformColliders[0], (void *)&Platform0Callback);
    func_020393c4(&mPlatformColliders[1], (void *)&Platform1Callback);
    func_020393c4(&mPlatformColliders[2], (void *)&Platform2Callback);
    func_020393c4(&mPlatformColliders[3], (void *)&Platform3Callback);

    for (int i = 0; i < 4; i++)
        mPlatformColliders[i].Enable(this);

    mHomePos = *(Vector3 *)&mPosX;

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
 * rebuilds both sets of matrices. The wobble cast, the volatile rest-pose
 * reads and the ray-element pointer writes are load-bearing; see leftovers. */
s32 daObjCvNewsLift_c::Behavior()
{
    Vector3 rayStart[3];
    Vector3 rayFrom[3];
    Vector3 rayTo[3];
    Vector3 ahead;
    s32 i;

    if (mBumped) {
        if (DecIfAbove0_Short(&mWobbleTimer)) {
            mWobblePhase += 0x1f00;
            {
                s16 sine = data_02082214[((u16)(s16)mWobblePhase >> 4) * 2];
                s32 amp = mWobbleTimer * 0x1a;
                s8 which = mLoweredPlatform;
                s16 wobble = (s16)(s32)(((long long)amp * sine + 0x800) >> 12);
                switch (which) {
                case 0: mAngleX = -wobble; break;
                case 1: mAngleX = wobble; break;
                case 2: mAngleZ = wobble; break;
                case 3: mAngleZ = -wobble; break;
                }
            }
        } else {
            s8 which = mLoweredPlatform;
            if (which == 0 || which == 2)
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
            rayStart[0].x = 0; rayStart[0].y = -0xa000; rayStart[0].z = 0x10e000;
            {
                Vector3 *ray = &rayStart[1];
                ray->x = 0x12c000; ray->y = -0xa000; ray->z = 0x10e000;
                ray = &rayStart[2];
                ray->x = -0x12c000; ray->y = -0xa000; ray->z = 0x10e000;
            }
            rayFrom[0].x = 0; rayFrom[0].y = 0; rayFrom[0].z = 0;
            {
                Vector3 *ray = &rayFrom[1];
                ray->x = 0; ray->y = 0; ray->z = 0;
                ray = &rayFrom[2];
                ray->x = 0; ray->y = 0; ray->z = 0;
            }
            rayTo[0].x = 0; rayTo[0].y = 0; rayTo[0].z = 0;
            {
                Vector3 *ray = &rayTo[1];
                ray->x = 0; ray->y = 0; ray->z = 0;
                ray = &rayTo[2];
                ray->x = 0; ray->y = 0; ray->z = 0;
            }
            Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
            {
                Vector3 *start = rayStart;
                Vector3 *from = rayFrom;
                Vector3 *to = rayTo;
                for (i = 0; i < 3; i++) {
                    ahead = *start;
                    ahead.z += 0x28000;
                    MulVec3Mat4x3(start, &data_020a0e68, from);
                    MulVec3Mat4x3(&ahead, &data_020a0e68, to);
                    AddVec3(from, (Vector3 *)&mPosX, from);
                    AddVec3(to, (Vector3 *)&mPosX, to);
                    start++; from++; to++;
                }
            }
            {
                dBgCh_Lin l0;
                dBgCh_Lin l1;
                dBgCh_Lin l2;
                {
                    s32 j = 0;
                    Vector3 *to = rayTo;
                    Vector3 *from = rayFrom;
                    dBgCh_Lin *line = &l0;
                    u32 hitSound = 0x1b;
                    for (; j < 3; j++) {
                        line->SetObjAndLine(*from, *to, this);
                        if (line->DetectClsn() && line->clsnDist <= 0x1f000) {
                            mHorzSpeed = 0;
                            mBumped = 1;
                            mWobbleTimer = 60;
                            Sound::PlayBank3(hitSound, *(Vector3 *)&mCamSpacePosX);
                        }
                        to++; from++; line++;
                    }
                }
                UpdatePos(0);
            }
        }
        if (!DecIfAbove0_Byte(&mTiltHoldTimer)) {
            mTargetRotation[0] = data_02092768[0];
            mTargetRotation[1] = data_02092768[1];
            mTargetRotation[2] = data_02092768[2];
            mTargetRotation[3] = data_02092768[3];
        }
    }
    Quaternion_SLerp(&mRotation[0], &mTargetRotation[0], 0x199, &mRotation[0]);
    if (Vec3_Equal((Vector3 *)&mPosX, &mHomePos))
        mResetTimer = 300;
    if (!DecIfAbove0_Short(&mResetTimer)) {
        *(Vector3 *)&mPosX = mHomePos;
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
    if (!mBumped && mResetTimer < 270) {
        mLoweredPlatform = -1;
        mHorzSpeed = 0;
    }
    UpdateModelTransforms();
    UpdateClsnTransforms();
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
    mMeshCollider.Disable();
    for (int i = 0; i < 4; i++)
        mPlatformColliders[i].Disable();
    ((SharedFilePtr *)&data_ov021_021149b0)->Release();
    ((SharedFilePtr *)&data_ov021_021149b8)->Release();
    ((SharedFilePtr *)&data_ov021_021149a0)->Release();
    ((SharedFilePtr *)&data_ov021_021149a8)->Release();
    return 1;
}

// @symbol _ZN17daObjCvNewsLift_c21UpdateModelTransformsEv
/* The render-side twin of UpdateClsnTransforms: the same pose, built at model
 * scale (positions shifted down by 3), written into the models' matrices. */
void daObjCvNewsLift_c::UpdateModelTransforms()
{
    Matrix4x3 tilt;
    int i;
    Matrix4x3 *mainMat;
    const Vector3 *offset;
    Vector3 hang;
    Vector3 pos;
    Vector3 scaled;

    Matrix4x3_FromQuaternion((Quaternion *)mRotation, &tilt);
    Vec3_Asr(&pos, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos.x, pos.y, pos.z);
    MulMat4x3Mat4x3(&tilt, &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    mModel.mat4x3 = data_020a0e68;
    mainMat = &mModel.mat4x3;
    offset = data_ov021_02114a20;
    for (i = 0; i < 4; i++) {
        hang = *offset;
        if (mLoweredPlatform == i && !mBumped)
            hang.y -= 0x1e000;
        data_020a0e68 = *mainMat;
        Vec3_Asr(&scaled, &hang, 3);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, scaled.x, scaled.y, scaled.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, data_ov021_02114740[i]);
        mPlatformModels[i].mat4x3 = data_020a0e68;
        offset++;
    }

    /* Same test as Render, and then nothing. An empty if drops it. */
    if (mResetTimer < 45 && (mResetTimer & 1))
        return;
}

// @symbol _ZN17daObjCvNewsLift_c20UpdateClsnTransformsEv
/* Rebuild the collision matrices: the main mesh from position, tilt and
 * facing, then each platform hung at its offset (lowered by 0x1e000 while
 * the player stands on it and the lift is not bumped). */
void daObjCvNewsLift_c::UpdateClsnTransforms()
{
    Matrix4x3 tilt;
    int i;
    const Vector3 *offset;
    daObjCvNewsLift_c *cursor;
    Matrix4x3 *platformMat;
    dBgW_KcMbg *platformCollider;
    Vector3 hang;

    Matrix4x3_FromQuaternion((Quaternion *)mRotation, &tilt);
    Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
    MulMat4x3Mat4x3(&tilt, &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    mClsnMat = data_020a0e68;
    mMeshCollider.Transform(mClsnMat, mAngleY);

    offset = data_ov021_02114a20;
    i = 0;
    cursor = this;
    platformMat = mPlatformMats;
    platformCollider = mPlatformColliders;
    for (; i < 4; i++) {
        hang = *offset;
        if (mLoweredPlatform == i && !mBumped)
            hang.y -= 0x1e000;
        data_020a0e68 = mClsnMat;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, hang.x, hang.y, hang.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, data_ov021_02114740[i]);
        /* this stepped one matrix at a time. An index or *platformMat misses. */
        cursor->mPlatformMats[0] = data_020a0e68;
        platformCollider->Transform(*platformMat, mAngleY);
        offset++;
        cursor = (daObjCvNewsLift_c *)((Matrix4x3 *)cursor + 1);
        platformMat++;
        platformCollider++;
    }
}
