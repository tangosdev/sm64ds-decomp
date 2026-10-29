//cpp
/* daObjEwbIce_c -- Chief Chilly's rink ice (EWB_ICE_A/B/C).
 *
 * Actor 0xaa is the arena, 0xab the big pieces, 0xac the small ones.
 * Each reports a point to daKing_Donketu_c, then the arena settles on the
 * course datum and the other two fall away.
 *
 * #pragma defer_codegen off is load-bearing. The out-of-line destructor is
 * the key function, so this TU emits D1, D0, then a D2 the cartridge does
 * not keep. The functions below stay in ROM order. The unit spans ov073
 * .text 0x02121f90..0x02122874 and closes with the touch helper, the
 * collision callback InitResources installs, and the three registry
 * factories (EWB_ICE_C, EWB_ICE_B, EWB_ICE_A).
 *
 * The four states are {enter, update} records of member-function pointers:
 *   data_ov073_021234b0  Report   StateReport_Enter   StateReport_Update
 *   data_ov073_021234d0  Arena    StateArena_Enter    StateArena_Update
 *   data_ov073_021234c0  Sink     StateSink_Enter     StateSink_Update
 *   data_ov073_021234a0  Settled  StateSettled_Enter  StateSettled_Update
 * (ov073 relocs 0x0212310c..0x02123144 tie each PMF literal to its body.)
 *
 * Notes (each measured with mwccarm 2004/b56):
 * - StateSettled_Enter: one Vector3 store then pos.y -= kSplashDrop is
 *   0x74 bytes. The cartridge is 0x8c: mPos is stored, stored again, then y
 *   is adjusted.
 * - StateArena_Update: mWobblePhase += 0x800 is 0x114 bytes. The cartridge
 *   is 0x120: the increment adds the pooled offset 0x332, and the sine index
 *   is a second sign-extending load through this+0x300.
 * - StateReport_Update: mWaypointsA[mSpawnIndex] / mWaypointsB component
 *   stores are 0xc8 bytes. The cartridge is 0xb0: one mla, then stores at
 *   +0x3e8 and +0x448. uniqueID is the word at boss+4.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjEwbIce_c_AssetId {
    ASSET_EWB_ICE_A_BMD = 0x05e7,
    ASSET_EWB_ICE_A_KCL = 0x05e8,
    ASSET_EWB_ICE_B_BMD = 0x05e9,
    ASSET_EWB_ICE_B_KCL = 0x05ea,
    ASSET_EWB_ICE_C_BMD = 0x05eb,
    ASSET_EWB_ICE_C_KCL = 0x05ec
};

#pragma defer_codegen off

#include "daObjEwbIce_c.h"
#include "daKing_Donketu_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "Particle__System.h"

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

/* The six file handles __sinit_d_a_obj_ewb_ice.cpp constructs below, one model
 * and one collision per EWB_ICE variant. The models'
 * ctor/dtor are the cartridge's func_02017acc / func_02017ab4 pair and the
 * collisions' func_02017b4c / SharedFilePtr_Destruct_Clsn; the manifest
 * aliases these. */
struct EwbIceModelFileHandle : SharedFilePtr {
    EwbIceModelFileHandle(u32 fileID);
    ~EwbIceModelFileHandle();
};
struct EwbIceCollisionFileHandle : SharedFilePtr {
    EwbIceCollisionFileHandle(u32 fileID);
    ~EwbIceCollisionFileHandle();
};

extern EwbIceModelFileHandle gEwbIceAModelFile;         /* model, file 0x5e7 */
extern EwbIceModelFileHandle gEwbIceBModelFile;         /* model, file 0x5e9 */
extern EwbIceModelFileHandle gEwbIceCModelFile;         /* model, file 0x5eb */
extern EwbIceCollisionFileHandle gEwbIceACollisionFile; /* clsn, file 0x5e8 */
extern EwbIceCollisionFileHandle gEwbIceBCollisionFile; /* clsn, file 0x5ea */
extern EwbIceCollisionFileHandle gEwbIceCCollisionFile; /* clsn, file 0x5ec */

/* The four {enter, update} state records __sinit_d_a_obj_ewb_ice.cpp fills
 * from the PMF literals in .data. */
typedef int (daObjEwbIce_c::*EwbIceStateFunc)();
extern EwbIceStateFunc data_ov073_02123134;
extern EwbIceStateFunc data_ov073_0212312c;
extern EwbIceStateFunc data_ov073_0212311c;
extern EwbIceStateFunc data_ov073_0212310c;
extern EwbIceStateFunc data_ov073_02123114;
extern EwbIceStateFunc data_ov073_02123124;
extern EwbIceStateFunc data_ov073_02123144;
extern EwbIceStateFunc data_ov073_0212313c;
extern daObjEwbIce_State data_ov073_021234b0;
extern daObjEwbIce_State data_ov073_021234c0;
extern daObjEwbIce_State data_ov073_021234d0;

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
void func_02012694(unsigned int id, const Vector3 *pos);
unsigned short DecIfAbove0_Short(unsigned short *p);
/* local extern: d_bg_w.cpp defines these; no header declares them. */
void func_020393d4(void *mesh, void *callback);
void func_020393c4(void *mesh, void *callback);
void Matrix4x3_FromRotationXYZExt(Matrix4x3 *mat, int x, int y, int z);
}

extern s16 data_02082214[];
extern int data_02092138;
extern u8 data_ov073_02123420[];
extern u8 data_ov073_02123424[];
extern char data_ov073_021231bc[];
extern char data_ov073_021231c0[];
extern char data_ov073_021231c4[];
extern daObjEwbIce_State data_ov073_021234a0;

// @symbol _ZN13daObjEwbIce_cD1Ev
// @symbol _ZN13daObjEwbIce_cD0Ev
daObjEwbIce_c::~daObjEwbIce_c() {}

// @symbol _ZN13daObjEwbIce_c19StateSettled_UpdateEv
/* Update after the splash. Nothing left to do. */
int daObjEwbIce_c::StateSettled_Update() {
    return 1;
}

// @symbol _ZN13daObjEwbIce_c18StateSettled_EnterEv
/* Enter the settled state: ice-splash particles under the piece, then bank-3 sound 0x172. */
int daObjEwbIce_c::StateSettled_Enter() {
    Vector3 pos;
    int y;

    mWobblePhase = 0;
    /* Written twice. One assignment then `pos.y -=` is 0x74 against the
       cartridge's 0x8c: the first copy is a separate store of mPos. */
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.x = mPosX;
    y = mPosY;
    pos.y = y;
    pos.z = mPosZ;
    pos.y = y - kSplashDrop;
    Particle::System::NewSimple(0x8b, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(0x8c, pos.x, pos.y, pos.z);
    func_02012694(0x172, &mCamSpacePos);
    return 1;
}

// @symbol _ZN13daObjEwbIce_c17StateArena_UpdateEv
/* Arena update. Wobble mAngleX while the timer runs; then drop onto the datum. */
int daObjEwbIce_c::StateArena_Update() {
    mWobbleAmp = kWobbleAmp;
    if (DecIfAbove0_Short(&mTimer) != 0) {
        /* The sine index sign-extends a second load of mWobblePhase. */
        s16 angle;
        s32 amp;
        int sine;

        mWobblePhase += kWobbleStep;
        angle = mWobblePhase;
        amp = mWobbleAmp;
        sine = data_02082214[(((int)((unsigned)(angle << 16) >> 16)) >> 4) * 2];
        mAngleX = (s16)(amp + FX_MUL(amp, sine));
        ApproachLinear(mWobbleAmp, 0, kWobbleApproach);
        return 1;
    }
    mAngleX = 0;
    mVertAccel = -kFallAccel;
    UpdatePos(0);
    if (mVariant != 0)
        goto done;
    {
        int surface = data_02092138 + kSurfaceAboveVoid;
        if (surface <= mPosY)
            goto done;
        mPosY = surface;
        mTimer = 0;
        mVertAccel = 0;
        mVertSpeed = 0;
        SetState(&data_ov073_021234a0);
    }
done:
    return 1;
}

// @symbol _ZN13daObjEwbIce_c16StateArena_EnterEv
/* Arena enter: a short timer, then fall no faster than 200 units/frame. */
int daObjEwbIce_c::StateArena_Enter() {
    mTimer = kArenaFrames;
    mTerminalVelocity = -kDeathBelowVoid;
    return 1;
}

// @symbol _ZN13daObjEwbIce_c16StateSink_UpdateEv
/* Big/small update. Hold still, creak, tilt, then destroy at the datum. */
int daObjEwbIce_c::StateSink_Update() {
    UpdatePos(0);
    if (mTimer < kSinkQuietBelow) {
        mVertAccel = 0;
        mVertSpeed = 0;
        if (mTimer == kSinkSoundAt)
            func_02012694(0x171, &mCamSpacePos);
        if (mTimer < kSinkSoundAt) {
            mVertAccel = -kFallAccel;
            ApproachLinear(mAngleX, kTiltTarget, kTiltStep);
        }
    }
    if (DecIfAbove0_Short(&mTimer) == 0 || data_02092138 - kDeathBelowVoid > mPosY) {
        MarkForDestruction();
    }
    return 1;
}

// @symbol _ZN13daObjEwbIce_c15StateSink_EnterEv
/* Big/small enter: long timer, accel and terminal both -20 units/frame. */
int daObjEwbIce_c::StateSink_Enter() {
    int neg = -kSinkSpeed;
    mTimer = kSinkFrames;
    mVertAccel = neg;
    mTerminalVelocity = neg;
    return 1;
}

// @symbol _ZN13daObjEwbIce_c18StateReport_UpdateEv
/* Initial update. The frame the timer reads 1, tell Chief Chilly where this piece is. */
int daObjEwbIce_c::StateReport_Update() {
    if (DecIfAbove0_Short(&mTimer) == 1) {
        /* mWaypointsA/B[mSpawnIndex] = pos recomputes the address per
           component (0xb0 -> 0xc8). The cartridge steps one scaled pointer
           (boss + mSpawnIndex * 0xc) and stores through its slot [0]. */
        char *boss = (char *)dActor_c::FindWithActorID(ACTOR_KING_DONKETU, 0);
        if (boss != 0) {
            switch (actorID) {
            case ACTOR_EWB_ICE_A:
                mBossID = ((dActor_c *)boss)->uniqueID;
                break;
            case ACTOR_EWB_ICE_B: {
                char *slot = boss + mSpawnIndex * 0xc;
                ((daKing_Donketu_c *)slot)->mWaypointsA[0].x = mPosX;
                ((daKing_Donketu_c *)slot)->mWaypointsA[0].y = mPosY;
                ((daKing_Donketu_c *)slot)->mWaypointsA[0].z = mPosZ;
                break;
            }
            case ACTOR_EWB_ICE_C: {
                char *slot = boss + mSpawnIndex * 0xc;
                ((daKing_Donketu_c *)slot)->mWaypointsB[0].x = mPosX;
                ((daKing_Donketu_c *)slot)->mWaypointsB[0].y = mPosY;
                ((daKing_Donketu_c *)slot)->mWaypointsB[0].z = mPosZ;
                break;
            }
            }
        }
    }
    return 1;
}

// @symbol _ZN13daObjEwbIce_c17StateReport_EnterEv
/* Enter the initial state. The report timer was armed in InitResources. */
int daObjEwbIce_c::StateReport_Enter() {
    return 1;
}

// @symbol _ZN13daObjEwbIce_c8SetStateEP17daObjEwbIce_State
/* Installs a state record and runs its enter function, if it has one. */
int daObjEwbIce_c::SetState(daObjEwbIce_State *state) {
    mState = state;
    if (mState->enter == 0)
        return 1;
    return (this->*(mState->enter))();
}

// @symbol _ZN13daObjEwbIce_c4KillEv
void daObjEwbIce_c::Kill() {
    Vector3 pos;
    Vector3 dust;

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += kKillRise;
    Particle::System::NewSimple(0xa, pos.x, pos.y, pos.z);
    dust = pos;
    PoofDustAt(dust);
    Sound::PlayBank3(0x41, mCamSpacePos);
}

// @symbol _ZN13daObjEwbIce_c16CleanupResourcesEv
int daObjEwbIce_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((EwbIceFileSlot *)data_ov073_021231bc)[mVariant].file->Release();
    ((EwbIceFileSlot *)data_ov073_021231c0)[mVariant].file->Release();
    return 1;
}

// @symbol _ZN13daObjEwbIce_c6RenderEv
int daObjEwbIce_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daObjEwbIce_c8BehaviorEv
int daObjEwbIce_c::Behavior() {
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
int daObjEwbIce_c::InitResources() {
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

    bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(
        *((EwbIceFileSlot *)data_ov073_021231bc)[mVariant].file);
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
        ((dBgW_KcMbg *)(&mMeshCollider))
            ->dBgW_KcMbg::SetFile(
                (KCL_File *)(kcl), mClsnMat, 0x1000, mAngleY,
                *(CLPS_Block *)(((EwbIceClpsSlot *)data_ov073_021231c4)[variant].clps));
    }
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    func_020393c4(&mMeshCollider, (void *)&StoodOnCallback);
    mMeshCollider.Enable(this);

    unk_338 = 0;
    mBossID = 0;
    SetState(&data_ov073_021234b0);
    return 1;
}

// @symbol _ZN13daObjEwbIce_c9OnStoodOnEP8dActor_c
/* Chief Chilly (actor 0xda) stood on a piece that is still in its initial
 * state: post the piece's spawn index to his waypoint cursor, and when it is
 * the waypoint he is after, move the piece on -- the arena (variant 0) and
 * the big/small pieces take different states. */
void daObjEwbIce_c::OnStoodOn(dActor_c *other) {
    daKing_Donketu_c *boss = static_cast<daKing_Donketu_c *>(other);
    int isBoss = other->actorID == ACTOR_KING_DONKETU;
    if (isBoss == 0)
        return;
    if (mState != &data_ov073_021234b0)
        return;
    boss->mWaypointCursor = mSpawnIndex;
    if (mSpawnIndex != boss->mLandedWaypoint)
        return;
    if (mVariant != 0)
        SetState(&data_ov073_021234c0);
    else
        SetState(&data_ov073_021234d0);
}

/* The stood-on callback InitResources installs in mMeshCollider's slot. The
 * slot passes three arguments; OnStoodOn wants the last two.
 * long_calls keeps the pooled absolute tail call. */
#pragma push
#pragma long_calls on
// @symbol _ZN13daObjEwbIce_c15StoodOnCallbackEP4dBgWPS_P8dActor_c
void daObjEwbIce_c::StoodOnCallback(dBgW *collider, daObjEwbIce_c *ice, dActor_c *other) {
    ice->OnStoodOn(other);
}
#pragma pop

// @symbol daObjEwbIce_c_classInit_EWB_ICE_C
/* Reconstructed source-style name: SM64DS proves daObjEwbIce_c through RTTI,
 * allocation size, vtable identity, and the EWB_ICE_C registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: CccSmallIce_Spawn.
 *
 * `new daObjEwbIce_c` is the whole sequence the loose factory spelled by
 * hand: fBase_c::operator new(0x33c), dBgActor_c's base constructor, then
 * the vptr store. */
extern "C" daObjEwbIce_c *daObjEwbIce_c_classInit_EWB_ICE_C(void) {
    return new daObjEwbIce_c;
}

// @symbol daObjEwbIce_c_classInit_EWB_ICE_B
/* Reconstructed source-style name: SM64DS proves daObjEwbIce_c through RTTI,
 * allocation size, vtable identity, and the EWB_ICE_B registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: CccBigIce_Spawn. */
extern "C" daObjEwbIce_c *daObjEwbIce_c_classInit_EWB_ICE_B(void) {
    return new daObjEwbIce_c;
}

// @symbol daObjEwbIce_c_classInit_EWB_ICE_A
/* Reconstructed source-style name: SM64DS proves daObjEwbIce_c through RTTI,
 * allocation size, vtable identity, and the EWB_ICE_A registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: CccArena_Spawn. */
extern "C" daObjEwbIce_c *daObjEwbIce_c_classInit_EWB_ICE_A(void) {
    return new daObjEwbIce_c;
}

/* Static-init globals (was the handwritten __sinit_ov073_02122d48 shard).
 * Definition order is the retail initializer's construction order: the
 * three model handles (files 0x5e7, 0x5e9, 0x5eb), the three collision
 * handles (files 0x5e8, 0x5ea, 0x5ec), then the four state records in
 * 234b0, 234c0, 234d0, 234a0 order, each copied from two PMF literals in
 * .data. */
// @symbol __sinit_d_a_obj_ewb_ice.cpp
EwbIceModelFileHandle gEwbIceAModelFile(ASSET_EWB_ICE_A_BMD);
EwbIceModelFileHandle gEwbIceBModelFile(ASSET_EWB_ICE_B_BMD);
EwbIceModelFileHandle gEwbIceCModelFile(ASSET_EWB_ICE_C_BMD);
EwbIceCollisionFileHandle gEwbIceACollisionFile(ASSET_EWB_ICE_A_KCL);
EwbIceCollisionFileHandle gEwbIceBCollisionFile(ASSET_EWB_ICE_B_KCL);
EwbIceCollisionFileHandle gEwbIceCCollisionFile(ASSET_EWB_ICE_C_KCL);

daObjEwbIce_State data_ov073_021234b0 = {data_ov073_02123134, data_ov073_0212312c};
daObjEwbIce_State data_ov073_021234c0 = {data_ov073_0212311c, data_ov073_0212310c};
daObjEwbIce_State data_ov073_021234d0 = {data_ov073_02123114, data_ov073_02123124};
daObjEwbIce_State data_ov073_021234a0 = {data_ov073_02123144, data_ov073_0212313c};
