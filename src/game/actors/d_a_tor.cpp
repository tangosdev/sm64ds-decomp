//cpp
/* daTor_c -- the sand tornado (registry profile TORNADO).
 * 11 function(s), .text 0x02136db0..0x021376bc.
 *
 * ROM evidence: _ZTS7daTor_c at ov096 0x02137a54; _ZTI at 0x02137a60 reads
 * [__si_class_type_info, that string, _ZTI8dActor_c]. The out-of-line
 * destructor is the key function, so this TU emits _ZTV/_ZTI/_ZTS. Under
 * defer_codegen off it lays down D1, D0, then a deadstripped D2, and .text in
 * source order -- this file is ROM-ascending, with the classInit factory
 * appended last.
 *
 * deslop leftovers:
 * - cstd::atan2, Particle::System::New, ModelAnim::SetAnim,
 *   TextureTransformer::SetFile, dCcAc_c::Init and dBgCh_Actr::Init take
 *   Fix12<int> by value and stay mangled extern "C" calls.
 * - dBgCh_Actr_UpdateContinuous_Veneer is the ROM's own tail-call thunk, not
 *   UpdateContinuous itself: the reloc points at the veneer symbol.
 * - The GetWallResult +4 in State1 is the unrecovered result struct's
 *   SurfaceInfo slot; dEnemyBase_c spells it the same way.
 * - State1 copies the player position through a raw (char*)player + 0x5c
 *   pointer: named mPosX/Y/Z loads come out one instruction shorter.
 * - unk_352 stays unnamed: zeroed by state 0 and read nowhere.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daTor_c_AssetId {
    ASSET_SAND_TORNADO_BCA = 0x041c,
    ASSET_SAND_TORNADO_BMD = 0x041d
};

#pragma defer_codegen off

#include "decl_common.h"
#include "daTor_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "SurfaceInfo.h"
#include "nitro/fx/fx.h"

extern "C" {
/* The texture animation InitResources hands to mTextureTransformer lives in
 * the level overlay, not here. ov096 relocs.txt lists the load at 0x021376b0
 * as ambiguous between ten level overlays; tools/overlay_residency.py settles
 * it to ov024, the only level whose tables load ov096. */
extern int data_ov024_02112968[];
extern s16 data_02082214[]; /* shared sin/cos lookup table */
s32 Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
s32 Vec3_Dist(const Vector3 *a, const Vector3 *b);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void dBgCh_Actr_UpdateContinuous_Veneer(void *collision);
s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 handle, u32 effect, s32 x, s32 y, s32 z, const void *direction,
    void *callback);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *self, void *file, int flags, int speed, unsigned int start);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    void *self, void *file, int flags, int speed, unsigned int start);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *self, void *actor, int radius, int height, unsigned int flags, unsigned int vuln);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *actor, int radius, int height, void *a, int b);
}

/* The tornado's file-scope SharedFilePtr objects, spelled through the same
   wrapper idiom the landed folds use: the handles' code is the veneer pair
   the cartridge already carries, so the wrappers get no bodies of their own.
   The constructors alias func_02017acc (model) and SharedFilePtr::Construct
   (animation) in this unit's manifest, and the destructors func_02017ab4
   and SharedFilePtr_Destruct_Anim. */
struct TorModelFilePtr : SharedFilePtr {
    int unk0;
    void *file;
    TorModelFilePtr(unsigned int id);
    ~TorModelFilePtr();
};

struct TorAnimationFileHandle : SharedFilePtr {
    int unk0;
    void *file;
    TorAnimationFileHandle(unsigned int id);
    ~TorAnimationFileHandle();
};

/* ------------------------------------------------------------------------
 * The model handle and the animation handle, in static-initializer order:
 * the initializer constructs each handle and registers its destructor.
 * InitResources feeds the model to Model::LoadFile and the animation to
 * dExtFrameCtrl_c::LoadFile.
 * ------------------------------------------------------------------------ */
// @symbol __sinit_d_a_tor.cpp
TorModelFilePtr   gSandTornadoModelFile(ASSET_SAND_TORNADO_BMD);   /* model */
TorAnimationFileHandle gSandTornadoAnimationFile(ASSET_SAND_TORNADO_BCA);   /* anim  */

bool ApproachLinear(short &value, short target, short step);

// @symbol _ZN7daTor_cD1Ev
// @symbol _ZN7daTor_cD0Ev
daTor_c::~daTor_c()
{
}

// @symbol _ZN7daTor_c10UpdateSpinEi
void daTor_c::UpdateSpin(s32 scale)
{
    u16 idx = (u16)mSpinPhase;
    s16 wave = data_02082214[(idx >> 4) * 2 + 1];
    s32 radiusScale = (wave + 0x1000) >> 1;
    s32 frameScale = FX_MUL(scale, 0x666);
    s32 radius = FX_MUL(radiusScale, 0x4cc);
    radius += 0x1000;
    mScaleX = FX_MUL(radius, frameScale);

    u16 idx2 = (u16)mSpinPhase;
    s16 wave2 = data_02082214[(idx2 >> 4) * 2 + 1];
    s32 heightScale = ((0x1000 - wave2) >> 2) + 0x800;
    mScaleY = FX_MUL(heightScale, frameScale);

    mScaleZ = mScaleX;
    mSpinPhase += 0x200;
    mHorzSpeed = 0xe000;
    mAngleY += 0x2c00;
    mdCcAc_c.radius = mScaleX * 0x514;
    mdCcAc_c.height = mScaleY * 0xfa0;
    mScaleX <<= 1;
    mScaleY <<= 2;
    mScaleZ <<= 1;
}

// @symbol _ZN7daTor_c6State2Ev
void daTor_c::State2()
{
    s32 scale = (0x3c - mStateTimer) << 12;
    if (scale >= 0) {
        UpdateSpin(scale / 0x3c);
        return;
    }
    mdCcAc_c.flags |= 1;
    mCaughtActor = 0;

    Player *player = ClosestPlayer();
    if (player) {
        dActor_c *actor = (dActor_c *)player;
        if (Vec3_HorzDist((const Vector3 *)&mHomePosX, &actor->mPos) > 0x9c4000)
            mState = 0;
        if (mStateTimer > 0x168)
            mState = 0;
        return;
    }
    mState = 0;
}

// @symbol _ZN7daTor_c6State1Ev
void daTor_c::State1()
{
    Vector3 playerPos;
    s16 angle;
    ++mChaseTimer;

    angle = Vec3_HorzAngle(&mPos, (const Vector3 *)&mHomePosX);
    mAngleToHome = angle;

    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x85, mCamSpacePos, 0);
    mHorzSpeed = 0x14000;

    Player *player = ClosestPlayer();
    if (player == 0)
        goto null_player;

    /* Leftover: raw pointer copy -- named loads change codegen; see file header. */
    {
        s32 *pos = (s32 *)((char *)player + 0x5c);
        playerPos.x = pos[0];
        playerPos.y = pos[1];
        playerPos.z = pos[2];
    }

    if (Vec3_HorzDist((const Vector3 *)&mHomePosX, &playerPos) < mChaseRange
        && mTriggerCount == 0
        && mChaseTimer < 0x384) {
        angle = Vec3_HorzAngle(&mPos, &playerPos);
        mAngleToPlayer = angle;
        ApproachLinear(mPrevAngleY, mAngleToPlayer, 0x200);
        /* ov002 helper wraps Player::IsState(tornado): still being carried. */
        if (mCaughtActor != 0 && func_ov002_020de328(mCaughtActor) != 0)
            ++mTriggerCount;
    } else {
        ApproachLinear(mPrevAngleY, mAngleToHome, 0x200);
        if (Vec3_HorzDist((const Vector3 *)&mHomePosX, &mPos) < 0xc8000)
            mState = 2;
    }
    goto cont;

null_player:
    mState = 2;
    return;

cont:
    if (Vec3_Dist(&mPos, &playerPos) > 0xbb8000 || mChaseTimer >= 0x384)
        mState = 2;

    UpdatePos(0);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    if (mWithMeshClsn.IsOnWall() != 0) {
        Vector3 normal;
        void *wall = mWithMeshClsn.GetWallResult();
        ((SurfaceInfo *)((char *)wall + 4))->CopyNormalTo(normal);
        mPrevAngleY = _ZN4cstd5atan2E5Fix12IiES1_(normal.x, normal.z);
    }

    UpdateSpin(0x1000);

    mParticleHandle0 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticleHandle0, 0x11f, mPosX, mPosY, mPosZ, 0, 0);
    mParticleHandle1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticleHandle1, 0x120, mPosX, mPosY, mPosZ, 0, 0);
}

// @symbol _ZN7daTor_c6State0Ev
void daTor_c::State0()
{
    if (mChaseTimer >= 0x384) {
        mPosX = mHomePosX;
        mPosY = mHomePosY;
        mPosZ = mHomePosZ;
    }

    if (mTriggerCount == 0) {
        mParticleHandle1 = 0;
        mParticleHandle0 = mParticleHandle1;
        mScaleX = 0;
        mScaleY = 0;
        mScaleZ = 0;
        unk_352 = 0;
        if (DistToCPlayer() < 0x5dc000)
            ++mTriggerCount;
        mStateTimer = 0;
        mdCcAc_c.flags &= ~1;
        mChaseTimer = 0;
    } else {
        mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x85, mCamSpacePos, 0);
        UpdateSpin((mStateTimer << 12) / 60);
        if (mStateTimer >= 0x3c)
            mState = 1;
    }
}

// @symbol _ZN7daTor_c16CleanupResourcesEv
int daTor_c::CleanupResources()
{
    gSandTornadoModelFile.Release();
    gSandTornadoAnimationFile.Release();
    return 1;
}

// @symbol _ZN7daTor_c6RenderEv
int daTor_c::Render()
{
    mTextureTransformer.Update(mModelAnim.data);
    mModelAnim.Render(&mScale);
    return 1;
}

// @symbol _ZN7daTor_c8BehaviorEv
int daTor_c::Behavior()
{
    int s = mState;
    switch (s) {
    case 0: State0(); break;
    case 1: State1(); break;
    case 2: State2(); break;
    }
    ++mStateTimer;
    if (s != mState) {
        mStateTimer = 0;
        mTriggerCount = 0;
    }
    unsigned int id = mdCcAc_c.otherOwner;
    if (id != 0 && (mdCcAc_c.hitFlags & 0x400000) != 0) {
        dActor_c *o = dActor_c::FindWithID(id);
        if (o != 0) {
            dActor_c *closest = ClosestWithActorID(0x135 /* ONIMASU */);
            if (closest == 0 || Vec3_Dist(&o->mPos, &closest->mPos) > 0x118000) {
                /* ov002 player-state helper: stores `this` into the player's
                   tornado slot and switches it into the tornado state. */
                if (func_ov002_020de33c((char *)o, (int)this) != 0)
                    mCaughtActor = o;
            }
        }
    }
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    Matrix4x3_FromTranslation(&mModelAnim.mat4x3, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    mModelAnim.dExtFrameCtrl_c::Advance();
    mTextureTransformer.Advance();
    return 1;
}

// @symbol _ZN7daTor_c13InitResourcesEv
int daTor_c::InitResources()
{
    /* The three Fix12-by-value calls below stay mangled (file header leftovers);
       member calls grow this function from 0x158 to 0x18c. */
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(gSandTornadoModelFile),
                       1, 0x15);
    dExtFrameCtrl_c::LoadFile(gSandTornadoAnimationFile);
    func_02016aac(&mModelAnim, 0x16, 1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (void *)gSandTornadoAnimationFile.file, 0, 0x1000, 0);
    TextureTransformer::Prepare(*(BMD_File *)gSandTornadoModelFile.file, *(BTA_File *)data_ov024_02112968);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        &mTextureTransformer, data_ov024_02112968, 0, 0x1000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0, 0, 0x200002, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x50000, 0x50000, 0, 0);

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mSpinPhase = 0;
    mState = 0;
    mTriggerCount = 0;
    mVertAccel = -0x1000;
    mTerminalVelocity = -0x1e000;
    {
        unsigned int t = param1 & 0xff;
        if (t == 0xff)
            mChaseRange = 0x5dc000;
        else
            mChaseRange = t * 0x64000;
    }
    mParticleHandle0 = 0;
    mParticleHandle1 = 0;
    return 1;
}

// @symbol daTor_c_classInit
extern "C" daTor_c *daTor_c_classInit()
{
    return new daTor_c();
}
