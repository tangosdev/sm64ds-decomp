//cpp
/**
 * The path lift (profiles KM2_SUSUMU and KM3_LIFT).
 *
 * Rides the level path named by param1's low nibble, node to node, at
 * 10.0 a frame. It waits at the first node until something the mesh
 * callback recognises stands on it for 20 frames, runs to the end of the
 * path, turns round, and runs back. Resource set 0 also sinks up to 30.0
 * while it is loaded and pauses 20 frames before each return leg; the
 * other set loops end to end without stopping.
 *
 * daLinelift2_c_classInit_KM2_SUSUMU / _KM3_LIFT are reconstructed
 * (RTTI daLinelift2_c, KM2_SUSUMU / KM3_LIFT registry profiles). Retail
 * does not store those spellings. Historical aliases:
 * SquareMetalNetLift_Spawn, ArrowPathLift_Spawn.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 *
 * Known limits:
 * - The three state functions, the path stepper and the mesh callback's
 *   target are members retaining their linker addresses as names; no ROM
 *   spelling survives. The state table's pointer-to-member records are the
 *   sinit's .data words, resolved through symbols.txt.
 * - func_ov091_02132380 stays free: it is stored as a dBgW callback word,
 *   so it cannot be a non-static member.
 * - dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled:
 *   both take Fix12<int> by value (notes/mwccarm-codegen.md 6az).
 * - func_020393d4 / func_020393c4 install dBgW's two callback words;
 *   dBgW carries no setter for them.
 * - (Vector3 *)&mPosX / &mTargetPosX / &unk_0a4: dActor_c and this class
 *   store the triples as scalars.
 * - func_ov091_02131cb0 stays free: it is typed on dActor_c and its body
 *   only touches the base's velocity triples (unk_0a4 / unk_0ac), which the
 *   shared header has not named yet.
 * - Behavior lowers mPosY by the sink offset and restores it before the
 *   model update, which the cartridge does too; the dead store is its own.
 */

#include "daLinelift2_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct KCL_File;
struct CLPS_Block;
struct Vec3;

typedef void (dBgActor_c::*StateFunc)();

extern "C" {
extern SharedFilePtr *data_ov091_021344fc[]; /* BMD, by mVariant */
extern SharedFilePtr *data_ov091_021344f4[]; /* KCL, by mVariant */
extern CLPS_Block *data_ov091_02134e5c[];    /* CLPS, by mVariant */
extern StateFunc data_ov091_021354e0[];      /* by mState */
extern s16 data_02082214[];                  /* sin/cos pairs */

void Vec3_Sub(Vector3 *res, const Vector3 *a, const Vector3 *b);
s32 Vec3_HorzLen(const Vector3 *v);
void AddVec3(Vec3 *a, Vec3 *b, Vec3 *out);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
int Vec3_Equal(void *a, void *b);
s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
void _Z14ApproachLinearRiii(int *value, int target, int step);
void func_02010da4(void *actor);
void func_020393d4(int *collider, int callback);
void func_020393c4(int *collider, int callback);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(
    dBgActor_c *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);

void func_ov091_02131cb0(dActor_c *self, const Vector3 *to, const Vector3 *from);
void func_ov091_02132380(void *a, void *b, void *c);
}

enum {
    kPlayerActorID = 0xbf,
    kWaitFrames = 20,
    kRideSpeed = 0xa000,     /* 10.0 */
};

/* How far the platform gives under a load, and how fast it gets there. */
static const int cSinkDepth = 0x1e000;
static const int cSinkRate  = 0x5000;

// @symbol daLinelift2_c_classInit_KM3_LIFT
extern "C" daLinelift2_c *daLinelift2_c_classInit_KM3_LIFT()
{
    return new daLinelift2_c();
}

// @symbol daLinelift2_c_classInit_KM2_SUSUMU
extern "C" daLinelift2_c *daLinelift2_c_classInit_KM2_SUSUMU()
{
    return new daLinelift2_c();
}

// @symbol func_ov091_02132380
/* The collider's second callback word: drop the collider, forward the
   pair. long_calls emits the pooled `ldr ip,[pc,#8]; bx ip` absolute
   tail-call; bracketed closed immediately because it is positional. */
#pragma long_calls on
extern "C" void func_ov091_02132380(void *a, void *b, void *c)
{
    ((daLinelift2_c *)b)->func_ov091_02132360((dActor_c *)c);
}
#pragma long_calls off

// @symbol _ZN13daLinelift2_c19func_ov091_02132360EP8dActor_c
void daLinelift2_c::func_ov091_02132360(dActor_c *other)
{
    unsigned char isPlayer = other->actorID == kPlayerActorID;
    if (isPlayer)
        mIsPressed = 1;
}

// @symbol _ZN13daLinelift2_c13InitResourcesEv
int daLinelift2_c::InitResources()
{
    mVariant = param1 >> 8;
    mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov091_021344fc[mVariant]), 1, -1);

    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*data_ov091_021344f4[mVariant]),
        &mClsnMat, 0x199, mAngleY, data_ov091_02134e5c[mVariant]);

    UpdateClsnPosAndRot();
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
    func_020393c4((int *)&mMeshCollider, (int)&func_ov091_02132380);

    mPathPtr.FromID(param1 & 0xf);
    mNodeCount = mPathPtr.NumNodes();
    mNodeIndex = 0;
    mBasePosX = mPosX;
    mBasePosY = mPosY;
    mBasePosZ = mPosZ;
    mPathPtr.GetNode(*(Vector3 *)&mTargetPosX, mNodeIndex);

    /* The first node is usually where we are standing; head for the next one. */
    if (Vec3_Equal(&mTargetPosX, &mBasePosX)) {
        mNodeIndex++;
        mPathPtr.GetNode(*(Vector3 *)&mTargetPosX, mNodeIndex);
    }

    mBaseAngleY = mAngleY;
    mSinkOffsetY = 0;
    mIsPressed = 0;
    return 1;
}

// @symbol _ZN13daLinelift2_c8BehaviorEv
int daLinelift2_c::Behavior()
{
    int old = mState;
    (this->*data_ov091_021354e0[old])();
    mStateTimer += 1;
    /* Restart the clock on a state change, and stop feeding the collider a
       velocity while the state is switching over. */
    if (old != mState) {
        mStateTimer = 0;
        func_020393d4((int *)&mMeshCollider, 0);
    } else {
        func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
    }
    if (mVariant == 0) {
        int saved = mPosY;
        _Z14ApproachLinearRiii(&mSinkOffsetY, mIsPressed ? cSinkDepth : 0, cSinkRate);
        mPosY -= mSinkOffsetY;
        mPosY = saved;
    }
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();
    mIsPressed = 0;
    return 1;
}

// @symbol _ZN13daLinelift2_c6RenderEv
int daLinelift2_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daLinelift2_c16CleanupResourcesEv
int daLinelift2_c::CleanupResources()
{
    data_ov091_021344fc[mVariant]->Release();
    data_ov091_021344f4[mVariant]->Release();
    mMeshCollider.Disable();
    return 1;
}

// @symbol _ZN13daLinelift2_c19func_ov091_02132000Ev
/* State 0: wait at the first node until something has stood on the lift
   for 20 frames, then set off along the path at 10.0 a frame. */
void daLinelift2_c::func_ov091_02132000()
{
    if (mIsPressed != 0) {
        if (mStateTimer <= kWaitFrames)
            return;
        mState = 1;
        mHorzSpeed = kRideSpeed;
        Vector3 target, base;
        target.x = mTargetPosX;
        target.y = mTargetPosY;
        target.z = mTargetPosZ;
        base.x = mBasePosX;
        base.y = mBasePosY;
        base.z = mBasePosZ;
        func_ov091_02131cb0(this, &target, &base);
        return;
    }
    mStateTimer = 0;
}

// @symbol _ZN13daLinelift2_c19func_ov091_02131f9cEv
/* State 1: run forward; at the path's end, turn round (state 2). */
void daLinelift2_c::func_ov091_02131f9c()
{
    if (func_ov091_02131db8() == -1) {
        mState = 2;
    }
    s16 old = mAngleY;
    mAngleY = mBaseAngleY;
    if (old != mAngleY) {
        func_020393d4((int *)&mMeshCollider, 0);
    } else {
        func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
    }
}

// @symbol _ZN13daLinelift2_c19func_ov091_02131ef0Ev
/* State 2: run back facing the other way. Resource set 0 pauses 20 frames
   first and settles in state 0 at the start; the other set loops. */
void daLinelift2_c::func_ov091_02131ef0()
{
    s16 old = mAngleY;
    mAngleY = mBaseAngleY + 0x8000;
    if (old != mAngleY) {
        func_020393d4((int *)&mMeshCollider, 0);
    } else {
        func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
    }
    if (mVariant == 0 && mStateTimer < kWaitFrames)
        return;
    if (func_ov091_02131db8() != -1)
        return;
    if (mVariant != 0) {
        mState = 1;
        return;
    }
    mState = 0;
    mAngleY = mBaseAngleY;
}

// @symbol _ZN13daLinelift2_c19func_ov091_02131db8Ev
/* One step along the path. Moves by the velocity; within half a step of the
   target node, snaps onto it, picks the next node in the direction of travel
   (mState 1 forward, otherwise back) and re-aims. Returns -1 when that runs
   off either end of the path, 1 on reaching a node, 0 in between. */
int daLinelift2_c::func_ov091_02131db8()
{
    int result;
    AddVec3((Vec3 *)&mPosX, (Vec3 *)&unk_0a4, (Vec3 *)&mPosX);
    if (Vec3_Dist((const Vector3 *)&mPosX, (const Vector3 *)&mTargetPosX) < (mHorzSpeed >> 1)) {
        result = 1;
        mBasePosX = mTargetPosX;
        mBasePosY = mTargetPosY;
        mBasePosZ = mTargetPosZ;
        mPosX = mBasePosX;
        mPosY = mBasePosY;
        mPosZ = mBasePosZ;
        if (mState == 1) {
            mNodeIndex++;
            if (mNodeIndex >= mNodeCount) {
                mNodeIndex = mNodeCount - 2;
                result = -1;
            }
        } else {
            mNodeIndex--;
            if (mNodeIndex < 0) {
                mNodeIndex = result;
                result = -1;
            }
        }
        mPathPtr.GetNode(*(Vector3 *)&mTargetPosX, (unsigned int)mNodeIndex);
        {
            Vector3 target, base;
            target.x = mTargetPosX;
            target.y = mTargetPosY;
            target.z = mTargetPosZ;
            base.x = mBasePosX;
            base.y = mBasePosY;
            base.z = mBasePosZ;
            func_ov091_02131cb0(this, &target, &base);
        }
        return result;
    }
    func_02010da4(this);
    return 0;
}

// @symbol func_ov091_02131cb0
/* Aim the velocity from `from` toward `to` at mHorzSpeed: pitch into
   mPrevAngleX, heading into mPrevAngleY, components into unk_0a4 /
   mVertSpeed / unk_0ac. */
extern "C" void func_ov091_02131cb0(dActor_c *self, const Vector3 *to, const Vector3 *from)
{
    Vector3 d;
    Vec3_Sub(&d, to, from);
    self->mPrevAngleY = _ZN4cstd5atan2E5Fix12IiES1_(d.x, d.z);
    self->mPrevAngleX = _ZN4cstd5atan2E5Fix12IiES1_(d.y, Vec3_HorzLen(&d));
    s32 horz = (s32)(((long long)self->mHorzSpeed
                      * data_02082214[((u16)self->mPrevAngleX >> 4) * 2 + 1] + 0x800) >> 12);
    self->unk_0a4 = (s32)(((long long)horz
                      * data_02082214[((u16)self->mPrevAngleY >> 4) * 2] + 0x800) >> 12);
    self->mVertSpeed = (s32)(((long long)self->mHorzSpeed
                      * data_02082214[((u16)self->mPrevAngleX >> 4) * 2] + 0x800) >> 12);
    self->unk_0ac = (s32)(((long long)horz
                      * data_02082214[((u16)self->mPrevAngleY >> 4) * 2 + 1] + 0x800) >> 12);
}

// @symbol _ZN13daLinelift2_cD1Ev
// @symbol _ZN13daLinelift2_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
