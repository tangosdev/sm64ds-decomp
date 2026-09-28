//cpp
/* daObjEwbIce_c -- Chief Chilly's rink ice (EWB_ICE_A/B/C).
 *
 * Actor 0xaa is the arena, 0xab the big pieces, 0xac the small ones.
 * Each reports a point to daKing_Donketu_c, then the arena settles on the
 * course datum and the other two fall away.
 *
 * #pragma defer_codegen off is load-bearing. The out-of-line destructor is
 * the key function, so this TU emits D1, D0, then a D2 the cartridge does
 * not keep. The functions below stay in ROM order.
 *
 * deslop leftovers:
 * - func_ov073_02122034: one Vector3 store then pos.y -= kSplashDrop is
 *   0x74 bytes. The cartridge is 0x8c: mPos is stored, stored again, then y
 *   is adjusted.
 * - func_ov073_021220c0: mWobblePhase += 0x800 is 0x114 bytes. The cartridge
 *   is 0x120: the increment adds the pooled offset 0x332, and the sine index
 *   is a second sign-extending load through this+0x300.
 * - func_ov073_021222ec: mWaypointsA[mSpawnIndex] / mWaypointsB component
 *   stores are 0xc8 bytes. The cartridge is 0xb0: one mla, then stores at
 *   +0x3e8 and +0x448. uniqueID is the word at boss+4.
 */

#pragma defer_codegen off

#include "daObjEwbIce_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"

struct CLPS_Block;

/* Column of the 0xc-stride file table. 021231bc / 021231c0 / 021231c4 are
   the model, collision and CLPS of row 0; each symbol is its own column. */
struct EwbIceFileSlot {
    SharedFilePtr *file;
    u8 pad[8];
};
struct EwbIceClpsSlot {
    CLPS_Block *clps;
    u8 pad[8];
};
typedef char EwbIceFileSlot_must_be_0xc[sizeof(EwbIceFileSlot) == 0xc ? 1 : -1];
typedef char EwbIceClpsSlot_must_be_0xc[sizeof(EwbIceClpsSlot) == 0xc ? 1 : -1];

struct daObjEwbIce_State {
    int (daObjEwbIce_c::*enter)();
    int (daObjEwbIce_c::*update)();
};
typedef char daObjEwbIce_State_must_be_0x10[sizeof(daObjEwbIce_State) == 0x10 ? 1 : -1];

enum {
    ACTOR_EWB_ICE_A = 0xaa,
    ACTOR_EWB_ICE_B = 0xab,
    ACTOR_EWB_ICE_C = 0xac,
    ACTOR_KING_DONKETU = 0xda,

    kReportFrames = 3,
    kArenaFrames = 0x64,
    kSinkFrames = 0x190,
    kSinkQuietBelow = 0x18d,
    kSinkSoundAt = 0x183,

    kWobbleAmp = 0x400,
    kWobbleStep = 0x800,
    kWobbleApproach = 0x40000,
    kSplashDrop = 0x12c000,
    kKillRise = 0x32000,
    kSurfaceAboveVoid = 0x96000,
    kDeathBelowVoid = 0xc8000,
    kFallAccel = 0xa000,
    kSinkSpeed = 0x14000,
    kTiltTarget = 0x2000,
    kTiltStep = 0x80
};

extern "C" {
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void func_02012694(unsigned int id, const Vector3 *pos);
unsigned short DecIfAbove0_Short(unsigned short *p);
extern s16 data_02082214[];
extern int data_02092138;
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *mesh, void *kcl, const Matrix4x3 *mat, int scale, short angY, void *clps);
void func_020393d4(void *mesh, void *callback);
void func_020393c4(void *mesh, void *callback);
extern u8 data_ov073_02123420[];
extern u8 data_ov073_02123424[];
extern char data_ov073_021231bc[];
extern char data_ov073_021231c0[];
extern char data_ov073_021231c4[];
extern daObjEwbIce_State data_ov073_021234a0;
extern void *data_ov073_021234b0;
int func_ov073_021223a4(daObjEwbIce_c *ice, daObjEwbIce_State *state);
int func_ov073_021227d0(void *a, void *b, void *c);
void Matrix4x3_FromRotationXYZExt(Matrix4x3 *mat, int x, int y, int z);
}

int ApproachLinear(int &value, int target, int step);
int ApproachLinear(short &value, short target, short step);

// @symbol _ZN13daObjEwbIce_cD1Ev
// @symbol _ZN13daObjEwbIce_cD0Ev
daObjEwbIce_c::~daObjEwbIce_c()
{
}

// @symbol func_ov073_0212202c
/* Update after the splash. Nothing left to do. */
extern "C" int func_ov073_0212202c(void)
{
    return 1;
}

// @symbol func_ov073_02122034
/* Enter the settled state: ice-splash particles under the piece, then bank-3 sound 0x172. */
extern "C" int func_ov073_02122034(char *raw)
{
    daObjEwbIce_c *ice = (daObjEwbIce_c *)raw;
    Vector3 pos;
    int y;

    ice->mWobblePhase = 0;
    /* Written twice. One assignment then `pos.y -=` is 0x74 against the
       cartridge's 0x8c: the first copy is a separate store of mPos. */
    pos.x = ice->mPosX;
    pos.y = ice->mPosY;
    pos.z = ice->mPosZ;
    pos.x = ice->mPosX;
    y = ice->mPosY;
    pos.y = y;
    pos.z = ice->mPosZ;
    pos.y = y - kSplashDrop;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8b, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8c, pos.x, pos.y, pos.z);
    func_02012694(0x172, (const Vector3 *)&ice->mCamSpacePosX);
    return 1;
}

// @symbol func_ov073_021220c0
/* Arena update. Wobble mAngleX while the timer runs; then drop onto the datum. */
extern "C" int func_ov073_021220c0(daObjEwbIce_c *ice)
{
    ice->mWobbleAmp = kWobbleAmp;
    if (DecIfAbove0_Short(&ice->mTimer) != 0) {
        /* mWobblePhase += 0x800 collapses both accesses to this+0x300 and
           shrinks this function 0x120 -> 0x114. The increment is an add of
           the pooled offset 0x332; the sine index sign-extends a second load. */
        unsigned short *phase = (unsigned short *)((int)ice + 0x332);
        s16 angle;
        s32 amp;
        int sine;

        *phase = (unsigned short)(*phase + kWobbleStep);
        angle = *(s16 *)((char *)((char *)ice + 0x300) + 0x32);
        amp = ice->mWobbleAmp;
        sine = data_02082214[(((int)((unsigned)(angle << 16) >> 16)) >> 4) * 2];
        ice->mAngleX = (s16)(amp + (int)(((long long)amp * sine + 0x800) >> 12));
        ApproachLinear(ice->mWobbleAmp, 0, kWobbleApproach);
        return 1;
    }
    ice->mAngleX = 0;
    ice->mVertAccel = -kFallAccel;
    ice->UpdatePos(0);
    if (ice->mVariant != 0)
        goto done;
    {
        int surface = data_02092138 + kSurfaceAboveVoid;
        if (surface <= ice->mPosY)
            goto done;
        ice->mPosY = surface;
        ice->mTimer = 0;
        ice->mVertAccel = 0;
        ice->mVertSpeed = 0;
        func_ov073_021223a4(ice, &data_ov073_021234a0);
    }
done:
    return 1;
}

// @symbol func_ov073_021221e0
/* Arena enter: a short timer, then fall no faster than 200 units/frame. */
extern "C" int func_ov073_021221e0(daObjEwbIce_c *ice)
{
    ice->mTimer = kArenaFrames;
    ice->mTerminalVelocity = -kDeathBelowVoid;
    return 1;
}

// @symbol func_ov073_02122200
/* Big/small update. Hold still, creak, tilt, then destroy at the datum. */
extern "C" int func_ov073_02122200(daObjEwbIce_c *ice)
{
    ice->UpdatePos(0);
    if (ice->mTimer < kSinkQuietBelow) {
        ice->mVertAccel = 0;
        ice->mVertSpeed = 0;
        if (ice->mTimer == kSinkSoundAt)
            func_02012694(0x171, (const Vector3 *)&ice->mCamSpacePosX);
        if (ice->mTimer < kSinkSoundAt) {
            ice->mVertAccel = -kFallAccel;
            ApproachLinear(ice->mAngleX, kTiltTarget, kTiltStep);
        }
    }
    if (DecIfAbove0_Short(&ice->mTimer) == 0 ||
        data_02092138 - kDeathBelowVoid > ice->mPosY) {
        ice->MarkForDestruction();
    }
    return 1;
}

// @symbol func_ov073_021222c8
/* Big/small enter: long timer, accel and terminal both -20 units/frame. */
extern "C" int func_ov073_021222c8(daObjEwbIce_c *ice)
{
    int neg = -kSinkSpeed;
    ice->mTimer = kSinkFrames;
    ice->mVertAccel = neg;
    ice->mTerminalVelocity = neg;
    return 1;
}

// @symbol func_ov073_021222ec
/* Initial update. The frame the timer reads 1, tell Chief Chilly where this piece is. */
extern "C" int func_ov073_021222ec(daObjEwbIce_c *ice)
{
    if (DecIfAbove0_Short(&ice->mTimer) == 1) {
        /* mWaypointsA/B[mSpawnIndex] = pos recomputes the address per
           component (0xb0 -> 0xc8). One scaled pointer, then +0x3e8 / +0x448,
           is what the cartridge stores. uniqueID is boss+4. */
        char *boss = (char *)dActor_c::FindWithActorID(ACTOR_KING_DONKETU, 0);
        if (boss != 0) {
            switch (ice->actorID) {
            case ACTOR_EWB_ICE_A:
                ice->mBossID = *(s32 *)(boss + 4);
                break;
            case ACTOR_EWB_ICE_B: {
                char *slot = boss + ice->mSpawnIndex * 0xc;
                *(s32 *)(slot + 0x3e8) = ice->mPosX;
                *(s32 *)(slot + 0x3ec) = ice->mPosY;
                *(s32 *)(slot + 0x3f0) = ice->mPosZ;
                break;
            }
            case ACTOR_EWB_ICE_C: {
                char *slot = boss + ice->mSpawnIndex * 0xc;
                *(s32 *)(slot + 0x448) = ice->mPosX;
                *(s32 *)(slot + 0x44c) = ice->mPosY;
                *(s32 *)(slot + 0x450) = ice->mPosZ;
                break;
            }
            }
        }
    }
    return 1;
}

// @symbol func_ov073_0212239c
/* Enter the initial state. The report timer was armed in InitResources. */
extern "C" int func_ov073_0212239c(void)
{
    return 1;
}

// @symbol func_ov073_021223a4
extern "C" int func_ov073_021223a4(daObjEwbIce_c *ice, daObjEwbIce_State *state)
{
    ice->mState = state;
    if (ice->mState->enter == 0)
        return 1;
    return (ice->*(ice->mState->enter))();
}

// @symbol _ZN13daObjEwbIce_c4KillEv
void daObjEwbIce_c::Kill()
{
    Vector3 pos;
    Vector3 dust;

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += kKillRise;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa, pos.x, pos.y, pos.z);
    dust = pos;
    PoofDustAt(dust);
    Sound::PlayBank3(0x41, *(const Vector3 *)&mCamSpacePosX);
}

// @symbol _ZN13daObjEwbIce_c16CleanupResourcesEv
int daObjEwbIce_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((EwbIceFileSlot *)data_ov073_021231bc)[mVariant].file->Release();
    ((EwbIceFileSlot *)data_ov073_021231c0)[mVariant].file->Release();
    return 1;
}

// @symbol _ZN13daObjEwbIce_c6RenderEv
int daObjEwbIce_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daObjEwbIce_c8BehaviorEv
int daObjEwbIce_c::Behavior()
{
    if (mState->update != 0)
        (this->*(mState->update))();
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
    UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN13daObjEwbIce_c13InitResourcesEv
int daObjEwbIce_c::InitResources()
{
    BMD_File *bmd;
    KCL_File *kcl;

    switch (actorID) {
    case ACTOR_EWB_ICE_A:
        mTimer = kReportFrames;
        mVariant = 0;
        break;
    case ACTOR_EWB_ICE_B:
        mTimer = kReportFrames;
        mSpawnIndex = data_ov073_02123424[0];
        data_ov073_02123424[0]++;
        mVariant = 1;
        break;
    case ACTOR_EWB_ICE_C:
        mTimer = kReportFrames;
        mSpawnIndex = data_ov073_02123420[0];
        data_ov073_02123420[0]++;
        mVariant = 2;
        break;
    }

    bmd = (BMD_File *)Model::LoadFile(*((EwbIceFileSlot *)data_ov073_021231bc)[mVariant].file);
    mModel.SetFile(bmd, 1, -1);
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
    UpdateClsnPosAndRot();

    /* mVariant reloaded after LoadFile is a second mul (0x1b8 -> 0x1c4).
       The byte has to stay live in a register across the call. */
    {
        u8 variant = mVariant;
        kcl = (KCL_File *)dBgW_Kc::LoadFile(*((EwbIceFileSlot *)data_ov073_021231c0)[variant].file);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
            ((EwbIceClpsSlot *)data_ov073_021231c4)[variant].clps);
    }
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    func_020393c4(&mMeshCollider, (void *)func_ov073_021227d0);
    mMeshCollider.Enable(this);

    unk_338 = 0;
    mBossID = 0;
    func_ov073_021223a4(this, (daObjEwbIce_State *)&data_ov073_021234b0);
    return 1;
}
