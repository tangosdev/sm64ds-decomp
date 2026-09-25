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
 * - The three state functions keep C linkage and placeholder names. They
 *   are pointer-to-members on the real class (ov091's static initializer
 *   copies them into data_ov091_021354e0, which Behavior calls through),
 *   but no ROM spelling survives.
 * - func_ov091_02132360 / 02132380 are the collider callback pair; 02132380
 *   is stored as a dBgW callback word, so it cannot be a non-static member.
 * - dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled:
 *   both take Fix12<int> by value (notes/mwccarm-codegen.md 6az).
 * - func_020393d4 / func_020393c4 install dBgW's two callback words;
 *   dBgW carries no setter for them.
 * - (Vector3 *)&mPosX / &mTargetPosX / &unk_0a4: dActor_c and this class
 *   store the triples as scalars.
 * - func_ov091_02131cb0 writes the velocity through dActor_c's unk_0a4 /
 *   unk_0ac, which the shared header has not named yet.
 * - Behavior lowers mPosY by the sink offset and restores it before the
 *   model update, which the cartridge does too; the dead store is its own.
 */

#include "daLinelift2_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct KCL_File;
struct CLPS_Block;

typedef void (dBgActor_c::*StateFunc)();

extern "C" {
extern SharedFilePtr *data_ov091_021344fc[]; /* BMD, by mVariant */
extern SharedFilePtr *data_ov091_021344f4[]; /* KCL, by mVariant */
extern CLPS_Block *data_ov091_02134e5c[];    /* CLPS, by mVariant */
extern StateFunc data_ov091_021354e0[];      /* by mState */
extern s16 data_02082214[];                  /* sin/cos pairs */

void Vec3_Sub(Vector3 *res, const Vector3 *a, const Vector3 *b);
s32 Vec3_HorzLen(const Vector3 *v);
void AddVec3(Vector3 *out, const Vector3 *a, const Vector3 *b);
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
int func_ov091_02131db8(daLinelift2_c *self);
void func_ov091_02131ef0(daLinelift2_c *self);
void func_ov091_02131f9c(daLinelift2_c *self);
void func_ov091_02132000(daLinelift2_c *self);
void func_ov091_02132360(daLinelift2_c *self, dActor_c *other);
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
    func_ov091_02132360((daLinelift2_c *)b, (dActor_c *)c);
}
#pragma long_calls off

// @symbol func_ov091_02132360
extern "C" void func_ov091_02132360(daLinelift2_c *self, dActor_c *other)
{
    unsigned char isPlayer = other->actorID == kPlayerActorID;
    if (isPlayer)
        self->mIsPressed = 1;
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

// @symbol func_ov091_02132000
/* State 0: wait at the first node until something has stood on the lift
   for 20 frames, then set off along the path at 10.0 a frame. */
extern "C" void func_ov091_02132000(daLinelift2_c *self)
{
    if (self->mIsPressed != 0) {
        if (self->mStateTimer <= kWaitFrames)
            return;
        self->mState = 1;
        self->mHorzSpeed = kRideSpeed;
        Vector3 target, base;
        target.x = self->mTargetPosX;
        target.y = self->mTargetPosY;
        target.z = self->mTargetPosZ;
        base.x = self->mBasePosX;
        base.y = self->mBasePosY;
        base.z = self->mBasePosZ;
        func_ov091_02131cb0(self, &target, &base);
        return;
    }
    self->mStateTimer = 0;
}

// @symbol func_ov091_02131f9c
/* State 1: run forward; at the path's end, turn round (state 2). */
extern "C" void func_ov091_02131f9c(daLinelift2_c *self)
{
    if (func_ov091_02131db8(self) == -1) {
        self->mState = 2;
    }
    s16 old = self->mAngleY;
    self->mAngleY = self->mBaseAngleY;
    if (old != self->mAngleY) {
        func_020393d4((int *)&self->mMeshCollider, 0);
    } else {
        func_020393d4((int *)&self->mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
    }
}

// @symbol func_ov091_02131ef0
/* State 2: run back facing the other way. Resource set 0 pauses 20 frames
   first and settles in state 0 at the start; the other set loops. */
extern "C" void func_ov091_02131ef0(daLinelift2_c *self)
{
    s16 old = self->mAngleY;
    self->mAngleY = self->mBaseAngleY + 0x8000;
    if (old != self->mAngleY) {
        func_020393d4((int *)&self->mMeshCollider, 0);
    } else {
        func_020393d4((int *)&self->mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
    }
    if (self->mVariant == 0 && self->mStateTimer < kWaitFrames)
        return;
    if (func_ov091_02131db8(self) != -1)
        return;
    if (self->mVariant != 0) {
        self->mState = 1;
        return;
    }
    self->mState = 0;
    self->mAngleY = self->mBaseAngleY;
}

// @symbol func_ov091_02131db8
/* One step along the path. Moves by the velocity; within half a step of the
   target node, snaps onto it, picks the next node in the direction of travel
   (mState 1 forward, otherwise back) and re-aims. Returns -1 when that runs
   off either end of the path, 1 on reaching a node, 0 in between. */
extern "C" int func_ov091_02131db8(daLinelift2_c *self)
{
    int result;
    AddVec3((Vector3 *)&self->mPosX, (const Vector3 *)&self->unk_0a4, (const Vector3 *)&self->mPosX);
    if (Vec3_Dist((const Vector3 *)&self->mPosX, (const Vector3 *)&self->mTargetPosX) < (self->mHorzSpeed >> 1)) {
        result = 1;
        self->mBasePosX = self->mTargetPosX;
        self->mBasePosY = self->mTargetPosY;
        self->mBasePosZ = self->mTargetPosZ;
        self->mPosX = self->mBasePosX;
        self->mPosY = self->mBasePosY;
        self->mPosZ = self->mBasePosZ;
        if (self->mState == 1) {
            self->mNodeIndex++;
            if (self->mNodeIndex >= self->mNodeCount) {
                self->mNodeIndex = self->mNodeCount - 2;
                result = -1;
            }
        } else {
            self->mNodeIndex--;
            if (self->mNodeIndex < 0) {
                self->mNodeIndex = result;
                result = -1;
            }
        }
        self->mPathPtr.GetNode(*(Vector3 *)&self->mTargetPosX, (unsigned int)self->mNodeIndex);
        {
            Vector3 target, base;
            target.x = self->mTargetPosX;
            target.y = self->mTargetPosY;
            target.z = self->mTargetPosZ;
            base.x = self->mBasePosX;
            base.y = self->mBasePosY;
            base.z = self->mBasePosZ;
            func_ov091_02131cb0(self, &target, &base);
        }
        return result;
    }
    func_02010da4(self);
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
