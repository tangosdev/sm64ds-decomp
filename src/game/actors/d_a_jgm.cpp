//cpp
/**
 * daJgm_c -- Lakitu (`jgm`, JUGEM 265), ov077.
 *
 * Hovers on a cloud and throws Spinies (actor 0x104), but not while four
 * are already out (CountSpinies). Five states: hover, throw, yoshi-mouth, spit, fall. ov077
 * is mixed (HEAVE_HO 238 / SPINY 260 / LAKITU 265). Not daC_Jugem_c
 * (C_JUGEM 235, ov085).
 *
 * daJgm_c_classInit is reconstructed (RTTI daJgm_c, JUGEM registry); retail
 * does not store that spelling. Historical aliases: Lakitu_Spawn and
 * Lakitu_SpawnInfo.
 *
 * leftovers:
 * - ROM-descending source order (factory first, OnYoshiTryEat last); the
 *   inline destructor emits D1 then D0 with no D2. The factory stays
 *   `return new daJgm_c()`, through the inherited fBase_c::operator new.
 * - common.h first is load-bearing: the flat m[12] Matrix4x3 keeps the
 *   12-word ldm/stm copies; UpdateModels writes translation as m[9..11].
 * - DaJgmVector3Words (not Vector3): a Vector3 stack local emits a
 *   vague-linkage ~Vector3 into this text-only TU. JgmVector3's
 *   declared-not-defined destructor aliases the ROM's canonical D1.
 * - mSpawnPosX stays punned: daJgm_c stores it as three scalars.
 * - dBgCh_Actr_UpdateDiscreteNoLava_veneer stays: the named
 *   UpdateDiscreteNoLava method is WRONG-DEST (ROM 0x02038420).
 * - Player::Bounce / Hurt stay mangled: each takes Fix12<int> by value
 *   (6az).
 * - dActor_c::IsTooFarAwayFromPlayer and cstd::fdiv have no header decl,
 *   so they are declared locally.
 * - Sound::PlayLong is the Sound.h call. d_a_jgmPlayLongHold keeps the five
 *   named parameters that hold the @NNN count. Unnaming them gives -5.
 * - The manifest pins eight @NNN statics, so every named local, named
 *   parameter and goto label ahead of __sinit counts (each advances the
 *   counter by one, measured). Control-flow shape moves the counter too:
 *   removing the UpdateFlight goto drops a label but measures +1. Names
 *   that also hold that count: bodyFile /
 *   heldFile and the loop counters in InitResources / CleanupResources,
 *   `frame` in UpdateThrowState, the `offset` references in the hover and
 *   throw collision updates, and the parameter names on the cstd::fdiv and
 *   IsTooFarAwayFromPlayer prototypes.
 * - OnTurnIntoEgg's R6Player and UpdateWallAndWater's R10dBgCh_Actr take
 *   reference spelling; a pointer emits identical ARM. SetState's `int`
 *   signedness is indistinguishable.
 * - func_0201267c (throw SFX 0xd2 at +0x74, mCamSpacePosX),
 *   data_02082214 (sin/cos pairs) and data_020a0e68 (arm9 scratch
 *   matrix) keep linker names.
 * - This TU claims .text only: the SharedFilePtr handles, the BCA/BTP
 *   pointer tables, the PMF state table data_ov077_02127bc4 and the
 *   collision-offset vector are defined at this file's tail (its sinit,
 *   outside the claimed range); g_profile_JUGEM is overlay data.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_jgm_AssetId {
    ASSET_JUGEM_BMD = 0x034c,
    ASSET_JUGEM_THROW_BCA = 0x034d,
    ASSET_JUGEM_THROW_BTP = 0x034e,
    ASSET_JUGEM_WAIT_BCA = 0x034f,
    ASSET_JUGEM_WAIT_BTP = 0x0350,
    ASSET_PAIPO_BMD = 0x0429,
    ASSET_TOGEZO_BMD = 0x042a
};

#include "common.h"
#include "daJgm_c.h"
#include "SpawnInfo.h"
#include "dBgCh_Gnd.h"
#include "Particle__System.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "SurfaceInfo.h"
#include "nitro/fx/fx.h"
#include "cstd.h"
#include "math/Approach.h"

/* Local three-word ABI value. Vector3 has a genuine non-trivial destructor;
 * these stack values are plain fixed-point words and must not instantiate its
 * otherwise unrelated vague-linkage D1 in this class TU. */
struct DaJgmVector3Words { Fix12i x, y, z; };

typedef char DaJgmVector3Words_size_must_be_0xc[
    sizeof(DaJgmVector3Words) == 0xc ? 1 : -1];

struct DaJgmMatrixWords { int words[12]; };

extern int _ZTV7daJgm_c[];

/* File-scope objects at the end of this file construct the seven resource
 * handles (model files 0x34c/0x429/0x42a, animation files 0x34f/0x34d,
 * texture-sequence files 0x350/0x34e), fill the five-pair state table and
 * set the collision-offset vector (0, -0x1e000, 0). mwcc emits
 * __sinit_d_a_jgm.cpp from those definitions. The wrapper names are local;
 * the handle constructors and destructors are the ROM resource-family
 * functions, aliased in the manifest. The vector is a JgmVector3 (below),
 * whose declared-not-defined destructor aliases the ROM's canonical
 * Vector3 D1. */
struct DaJgmModelFile : SharedFilePtr {
    DaJgmModelFile(u32 fileID);
    ~DaJgmModelFile();
};

struct DaJgmAnimationFilePtr : SharedFilePtr {
    DaJgmAnimationFilePtr(u32 fileID);
    ~DaJgmAnimationFilePtr();
};

struct DaJgmTexSequenceFilePtr : SharedFilePtr {
    DaJgmTexSequenceFilePtr(u32 fileID);
    ~DaJgmTexSequenceFilePtr();
};

/* Collision-offset vector: three plain words built by an inline constructor
 * (mwcc inlines the stores into __sinit) with a declared-not-defined
 * destructor (never ~Vector3: a Vector3 here would instantiate its
 * vague-linkage D1 into this TU, which deliberately avoids it); the
 * destructor alias points at the ROM's canonical copy. */
struct JgmVector3 {
    int x, y, z;
    JgmVector3(int a, int b, int c)
    {
        x = a;
        y = b;
        z = c;
    }
    ~JgmVector3();
};

extern "C" {
extern DaJgmModelFile data_ov077_02127b50;
extern DaJgmModelFile data_ov077_02127b48;
extern DaJgmModelFile data_ov077_02127b38;
extern DaJgmAnimationFilePtr data_ov077_02127b28;
extern DaJgmAnimationFilePtr data_ov077_02127b40;
extern DaJgmTexSequenceFilePtr data_ov077_02127b20;
extern DaJgmTexSequenceFilePtr data_ov077_02127b30;
extern SharedFilePtr *data_ov077_02127238[];
extern SharedFilePtr *data_ov077_02127230[];
extern DaJgmStateHandlers data_ov077_02127bc4[];
extern JgmVector3 data_ov077_02127b88;
extern DaJgmStateHandler data_ov077_021277e0;
extern DaJgmStateHandler data_ov077_02127828;
extern DaJgmStateHandler data_ov077_02127820;
extern DaJgmStateHandler data_ov077_021277e8;
extern DaJgmStateHandler data_ov077_02127818;
extern DaJgmStateHandler data_ov077_02127800;
extern DaJgmStateHandler data_ov077_021277f0;
extern DaJgmStateHandler data_ov077_02127808;
extern DaJgmStateHandler data_ov077_02127810;
extern DaJgmStateHandler data_ov077_021277f8;
extern s16 data_02082214[];
extern Matrix4x3 data_020a0e68;
extern Matrix4x3 IDENTITY_MATRIX4X3;

extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);

extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 angY);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void *a, void *b, void *out);
extern void Vec3_LslInPlace(void *v, int sh);
extern void Vec3_Asr(void *d, void *s, int sh);

/* Parameter names keep this TU's @NNN. */
typedef int BgchInitAt_(dBgCh_Actr *collision, dActor_c *actor, Fix12i radius, Fix12i height,
                        void *a, void *b);
/* local extern: kept as a count holder: its five named parameters hold
 * the @NNN count. Sound.h's PlayLong emits the same bytes. */
extern void d_a_jgmPlayLongHold(unsigned int handle, unsigned int a, unsigned int id,
                                const Vector3 *pos, unsigned int e);
extern void d_a_jgmNewSimpleHold(unsigned int n, int x, int y, int z);
}

// @symbol daJgm_c_classInit
extern "C" daJgm_c *daJgm_c_classInit()
{
    return new daJgm_c();
}

extern "C" SpawnInfo<daJgm_c> g_profile_JUGEM = {
    daJgm_c_classInit,
    0x0109,
    0x007c,
    0x10000006,
    0x0001e000,
    0x00064000,
    0x01000000,
    0x01000000
};

// @symbol _ZN7daJgm_c13OnTurnIntoEggER6Player
void daJgm_c::OnTurnIntoEgg(Player &player)
{
    if (player.IsCollectingCap())
        GivePlayerCoins(player, 5, 0);
    else
        player.RegisterEggCoinCount(5, 0, 0);
    KillAndTrackInDeathTable();
}

// @symbol _ZN7daJgm_c13InitResourcesEv
s32 daJgm_c::InitResources()
{
    dExtSimpleModel_c::LoadFile(data_ov077_02127b38);
    BMD_File *bodyFile = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov077_02127b50);
    mAnmModel.SetFile(bodyFile, 1, 1);
    BMD_File *heldFile = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov077_02127b48);
    mModel.SetFile(heldFile, 1, 1);
    for (int i = 0; i < 2; i++)
        dExtFrameCtrl_c::LoadFile(*data_ov077_02127238[i]);
    for (int i = 0; i < 2; i++) {
        DaJgmTexSequenceFilePtr *t =
            (DaJgmTexSequenceFilePtr *)data_ov077_02127230[i];
        dExtAnmTexPat_c::LoadFile(*t);
        dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov077_02127b50.filePtr, *(BTP_File *)t->filePtr);
    }
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mdCcAcPos_c.Init(this, *((Vector3 *)&data_ov077_02127b88), 0x41000, 0x78000, 0x200002, 0x6eff0);
    mWithMeshClsn.Init(this, 0x2d000, 0x2d000, 0, 0);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mSoundHandle = 0;

    SetState(0);
    mMatrix = IDENTITY_MATRIX4X3;
    UpdateModels();
    return 1;
}

// @symbol _ZN7daJgm_c8BehaviorEv
s32 daJgm_c::Behavior()
{
    int v = param1;
    if (IsTooFarAwayFromPlayer(v ? 0x1068000 : 0x7d0000))
        return 1;
    UpdateState();
    UpdateModels();
    return 1;
}

// @symbol _ZN7daJgm_c6RenderEv
s32 daJgm_c::Render()
{
    int b = (int)((mFlags & 0x40000) != 0);
    if (b != 0)
        return 1;
    mTextureSequence.Update(mAnmModel.data);
    mAnmModel.Render(0);
    if (mState == 1) {
        unsigned int v = ((unsigned int)(mAnmModel.currFrame << 4)) >> 0x10;
        if (v >= 0x19 && v <= 0x3a)
            mModel.Render(0);
    }
    return 1;
}

// @symbol _ZN7daJgm_c16OnPendingDestroyEv
void daJgm_c::OnPendingDestroy()
{
}

// @symbol _ZN7daJgm_c16CleanupResourcesEv
s32 daJgm_c::CleanupResources()
{
    data_ov077_02127b50.Release();
    data_ov077_02127b48.Release();
    data_ov077_02127b38.Release();

    for (int i = 0; i < 2; i++)
        data_ov077_02127238[i]->Release();

    for (int i = 0; i < 2; i++)
        data_ov077_02127230[i]->Release();

    return 1;
}

// @symbol _ZN7daJgm_c8SetStateEi
void daJgm_c::SetState(int state)
{
    mStateHandlers = &data_ov077_02127bc4[state];
    EnterState();
}

// @symbol _ZN7daJgm_c10EnterStateEv
void daJgm_c::EnterState()
{
    DaJgmStateHandler *handler = &mStateHandlers->enter;
    (this->**handler)();
}

// @symbol _ZN7daJgm_c11UpdateStateEv
void daJgm_c::UpdateState()
{
    DaJgmStateHandler *handler = &mStateHandlers->update;
    (this->**handler)();
}

// @symbol _ZN7daJgm_c15EnterHoverStateEv
s32 daJgm_c::EnterHoverState()
{
    mHorzSpeed = 0xc000;
    mAnmModel.SetAnim((BCA_File *)data_ov077_02127b28.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mTextureSequence.SetFile(*(BTP_File *)data_ov077_02127b20.filePtr, 0, 0x1000, 0);
    mTextureSequence.speed = 0x1000;
    mTimer = 0x96;
    mState = 0;
    return 1;
}

// @symbol _ZN7daJgm_c16UpdateHoverStateEv
s32 daJgm_c::UpdateHoverState()
{
    mHorzSpeed = 0xc000;
    if (DecIfAbove0_Byte(&mTimer) == 0) {
        /* Nested on purpose: one flat && chain is 12 bytes shorter. */
        Player *p = ClosestNonVanishPlayer();
        if (p != 0) {
            if (p->mPosY < mPosY && Vec3_HorzDist(&mPos, &p->mPos) < 0x190000 && CountSpinies() < 4)
                SetState(1);
            else
                mTimer = 0x96;
        } else {
            mTimer = 0x96;
        }
    }
    UpdateFlight();
    UpdateHoverBob();
    mAnmModel.Advance();
    mTextureSequence.Advance();
    HandlePlayerCollision();
    {
        const JgmVector3 &offset = data_ov077_02127b88;
        DaJgmVector3Words pos;
        pos.y = offset.y + mBobOffsetY;
        pos.x = offset.x;
        pos.z = offset.z;
        mdCcAcPos_c.SetPosRelativeToActor(*(Vector3 *)&pos);
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x182, *(Vector3 *)&mCamSpacePos, 0);
    return 1;
}

// @symbol _ZN7daJgm_c15EnterThrowStateEv
s32 daJgm_c::EnterThrowState()
{
    mAnmModel.SetAnim((BCA_File *)data_ov077_02127b40.filePtr, 0, 0x1000, 0);
    mAnmModel.SetFlags(0x40000000);
    mAnmModel.speed = 0x1000;
    mAnmModel.currFrame = 0;
    mTextureSequence.SetFile(*(BTP_File *)data_ov077_02127b30.filePtr, 0, 0x1000, 0);
    mTextureSequence.SetFlags(0x40000000);
    mTextureSequence.speed = 0x1000;
    mTextureSequence.currFrame = 0;
    mState = 1;
    return 1;
}

// @symbol _ZN7daJgm_c16UpdateThrowStateEv
s32 daJgm_c::UpdateThrowState()
{
    unsigned int frame = (unsigned int)(mAnmModel.currFrame << 4) >> 16;
    if (frame == 0x3a) {
        dActor_c::Spawn(0x104, 0, *(Vector3 *)&mThrowPosX, &mAngle, mAreaId, -1);
        func_0201267c(0xd2, &mCamSpacePos);
    }
    if (mAnmModel.Finished())
        SetState(0);
    UpdateFlight();
    UpdateHoverBob();
    mAnmModel.Advance();
    mTextureSequence.Advance();
    HandlePlayerCollision();
    {
        const JgmVector3 &offset = data_ov077_02127b88;
        DaJgmVector3Words pos;
        pos.y = offset.y + mBobOffsetY;
        pos.x = offset.x;
        pos.z = offset.z;
        mdCcAcPos_c.SetPosRelativeToActor(*(Vector3 *)&pos);
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x182, *(Vector3 *)&mCamSpacePos, 0);
    return 1;
}

// @symbol _ZN7daJgm_c20EnterYoshiMouthStateEv
s32 daJgm_c::EnterYoshiMouthState()
{
    mHorzSpeed = 0;
    mdCcAcPos_c.Clear();
    mState = 2;
    return 1;
}

// @symbol _ZN7daJgm_c21UpdateYoshiMouthStateEv
s32 daJgm_c::UpdateYoshiMouthState()
{
    if (((mFlags & 0x40000) ? 1 : 0) != 0) {
        dActor_c *carrier = mCarryActor;
        s32 *carrierPos = &carrier->mPosX;
        mPosX = carrierPos[0];
        mPosY = carrierPos[1];
        mPosZ = carrierPos[2];
    }
    {
        int flags = mFlags;
        if (((flags & 0x80000) ? 1 : 0) != 0)
            SetState(3);
        else if (((flags & 0x20000) ? 1 : 0) == 0
                 && ((flags & 0x40000) ? 1 : 0) == 0)
            SetState(0);
    }
    return 1;
}

// @symbol _ZN7daJgm_c14EnterSpitStateEv
s32 daJgm_c::EnterSpitState()
{
    dActor_c *carrier;
    int *pPosX;
    int *pPosY;
    int *pPosZ;
    int *src;
    int idx;
    s16 sinv;
    s16 cosv;

    mFlags &= ~0x80000;

    carrier = mCarryActor;
    pPosX = &mPosX;
    mHorzSpeed = carrier->mHorzSpeed + 0xa000;

    carrier = mCarryActor;
    pPosY = &mPosY;
    pPosZ = &mPosZ;
    mPrevAngleY = carrier->mAngleY;

    carrier = mCarryActor;

    src = &carrier->mPosX;
    mPosX = src[0];
    mPosY = src[1];
    mPosZ = src[2];

    idx = ((int)(u16)mPrevAngleY) >> 4;
    sinv = data_02082214[idx * 2];
    *pPosX = *pPosX + FX_MUL(sinv, 0x50000);
    *pPosY = *pPosY + 0x50000;
    idx = ((int)(u16)mPrevAngleY) >> 4;
    cosv = data_02082214[idx * 2 + 1];
    *pPosZ = *pPosZ + FX_MUL(cosv, 0x50000);

    mAngVelY = 0x7000;
    mTimer = 0x1e;
    mCarryActor = 0;
    mState = 3;
    return 1;
}

// @symbol _ZN7daJgm_c15UpdateSpitStateEv
s32 daJgm_c::UpdateSpitState()
{
    mAngleY = (short)(mAngleY + mAngVelY);
    ApproachLinear2(mAngVelY, 0, 0x300);
    UpdatePos(&mdCcAcPos_c);
    UpdateWallAndWater(mWithMeshClsn);
    HandlePlayerCollision();
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    if (!DecIfAbove0_Byte(&mTimer))
        SetState(0);
    return 1;
}

// @symbol _ZN7daJgm_c14EnterFallStateEv
s32 daJgm_c::EnterFallState()
{
    Sound::PlayBank0(9, mCamSpacePos);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mTimer = 0x2d;
    mAnmModel.SetAnim((BCA_File *)data_ov077_02127b28.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x4000;
    {
        int aimHeight = OnAimedAtWithEgg();
        Particle::System::NewSimple(0x43, mPosX, mPosY + aimHeight, mPosZ);
        int aimHeightAgain = OnAimedAtWithEgg();
        Particle::System::NewSimple(0x44, mPosX, mPosY + aimHeightAgain, mPosZ);
    }
    mState = 4;
    return 1;
}

// @symbol _ZN7daJgm_c15UpdateFallStateEv
s32 daJgm_c::UpdateFallState()
{
    mAngleX = (short)(mAngleX - 0x1000);
    mAnmModel.Advance();
    UpdatePos(&mdCcAcPos_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround() || !DecIfAbove0_Byte(&mTimer))
        DieAndDropCoins();
    return 1;
}

// @symbol _ZN7daJgm_c12UpdateModelsEv
void daJgm_c::UpdateModels()
{
    int b = (int)((mFlags & 0x40000) != 0);
    if (b)
        return;

    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = (mPosY + mBobOffsetY) >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;

    if (mState == 4) {
        data_020a0e68 = mAnmModel.mat4x3;
        int y1 = OnAimedAtWithEgg() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
        int y2 = (-OnAimedAtWithEgg()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);
        mAnmModel.mat4x3 = data_020a0e68;
    }

    mMatrix.m[9] = mPosX >> 3;
    mMatrix.m[10] = mPosY >> 3;
    mMatrix.m[11] = mPosZ >> 3;

    DropShadowRadHeight(mShadowModel, mMatrix, 0x5a000, 0x320000, 0xf);

    if (mState != 1)
        return;

    u32 t = (u32)(mAnmModel.currFrame << 4) >> 0x10;
    if (t < 0x19)
        return;
    if (t > 0x3a)
        return;

    struct { int t[3]; int v[3]; } lv;
    lv.t[0] = -0x2000;
    lv.t[1] = 0x1800;
    lv.t[2] = 0x1000;

    data_020a0e68 = mAnmModel.mat4x3;
    MulMat4x3Mat4x3(&mAnmModel.data.transforms[3], &data_020a0e68, &data_020a0e68);

    mThrowPosX = data_020a0e68.m[9];
    mThrowPosY = data_020a0e68.m[10];
    mThrowPosZ = data_020a0e68.m[11];

    Vec3_LslInPlace(&mThrowPosX, 3);
    Vec3_Asr(lv.v, &mThrowPosX, 3);

    Matrix4x3_FromTranslation(&data_020a0e68, lv.v[0], lv.v[1], lv.v[2]);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, lv.t[0], lv.t[1], lv.t[2]);

    *(DaJgmMatrixWords *)&mModel.mat4x3 = *(DaJgmMatrixWords *)&data_020a0e68;
}

// @symbol _ZN7daJgm_c18UpdateWallAndWaterER10dBgCh_Actr
void daJgm_c::UpdateWallAndWater(dBgCh_Actr &collision)
{
    DaJgmVector3Words nrm;
    DaJgmVector3Words pos;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&collision);
    if (collision.IsOnWall() != 0) {
        SurfaceInfo *info =
            (SurfaceInfo *)((char *)collision.GetWallResult() + 4);
        info->CopyNormalTo(*(Vector3 *)&nrm);
        mTurnDir ^= 1;
    }
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x64000;
    {
        dBgCh_Gnd rg;
        rg.SetObjAndPos(*(Vector3 *)&pos, 0);
        rg.StartDetectingWater();
        if (rg.DetectClsn() != 0) {
            int yy = rg.clsnY + 0x3c000;
            if (mPosY < yy)
                mPosY = yy;
        }
        rg.StopDetectingWater();
    }
}

// @symbol _ZN7daJgm_c21HandlePlayerCollisionEv
void daJgm_c::HandlePlayerCollision()
{
    dActor_c *other;
    int b;

    if (FindEgg(mdCcAcPos_c) != 0) {
        Sound::PlayBank0(9, mCamSpacePos);
        DieAndDropCoins();
        return;
    }

    {
        unsigned int id = mdCcAcPos_c.otherOwner;
        if (id == 0)
            return;
        other = dActor_c::FindWithID(id);
    }
    if (other == 0)
        return;

    b = (int)(other->actorID == 0xbf);
    if (b == 0)
        return;

    b = (int)((mFlags & 0x20000) != 0);
    if (b != 0) {
        SetState(2);
        return;
    }

    Player *player = (Player *)other;
    if ((mdCcAcPos_c.hitFlags & 0x66fe0)
        || BumpedUnderneathByPlayer(*player) != 0
        || player->IsOnShell() != 0
        || player->mIsMetal != 0) {
        Sound::PlayBank0(9, mCamSpacePos);
        DieAndDropCoins();
        return;
    }

    if (mdCcAcPos_c.hitFlags & 0x10) {
        mPrevAngleY = Vec3_HorzAngle(&player->mPos, &mPos);
        mAngleY = (short)(mPrevAngleY + 0x8000);
        player->IncMegaKillCount();
        SetState(4);
        return;
    }

    if (JumpedOnByPlayer(mdCcAcPos_c, *player) != 0) {
        player->Bounce(0x28000);
        DieAndDropCoins();
        return;
    }

    if (mState == 3)
        return;

    {
        DaJgmVector3Words v;
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        player->Hurt(*(Vector3 *)&v, 2, 0xc000, 1, 0, 1);
    }
}

// @symbol _ZN7daJgm_c15DieAndDropCoinsEv
void daJgm_c::DieAndDropCoins()
{
    DaJgmVector3Words t;
    t.x = mPosX;
    t.y = mPosY;
    t.z = mPosZ;
    SpawnCoins(*(const Vector3 *)&t, 5, 0x3000, 0);
    PoofDust();
    KillAndTrackInDeathTable();
}

// @symbol _ZN7daJgm_c12UpdateFlightEv
void daJgm_c::UpdateFlight() {
    /* The goto is kept: the goto-free if/else emits 4 more bytes. */
    Player *p = ClosestNonVanishPlayer();
    s32 *target;
    int chaseRange;
    if (p != 0) {
        chaseRange = (param1 != 0) ? 0x1068000 : 0x7d0000;
        if (Vec3_HorzDist((Vector3 *)&mSpawnPosX, &p->mPos) < chaseRange) {
            int d = mSpawnPosY - p->mPosY;
            if (d < 0)
                d = -d;
            if (d < 0x5dc000) {
                target = &p->mPosX;
                goto haveTarget;
            }
        }
        target = &mSpawnPosX;
    } else {
        target = &mSpawnPosX;
    }
haveTarget:
    int hd = Vec3_HorzDist(&mPos, (Vector3 *)target);
    short ha = Vec3_HorzAngle(&mPos, (Vector3 *)target);
    ApproachLinear(mAngleY, ha, 0x5e8);
    if (mTurnDir != 0)
        mPrevAngleY = (short)(mAngleY - TurnOffsetFromDist(hd));
    else
        mPrevAngleY = (short)(mAngleY + TurnOffsetFromDist(hd));
    ApproachLinear(mPosY, target[1] + 0x12c000, 0x2000);
    UpdatePos(&mdCcAcPos_c);
    UpdateWallAndWater(mWithMeshClsn);
}

// @symbol _ZN7daJgm_c18TurnOffsetFromDistEi
int daJgm_c::TurnOffsetFromDist(int dist)
{
    if (dist > 0x190000)
        return 0;
    int q = cstd::fdiv(0x4000, 0xc8000);
    long long m = (long long)q * dist;
    m += 0x800;
    int r = (int)(m >> 12);
    r = 0x8000 - r;
    return (short)r;
}

// @symbol _ZN7daJgm_c12CountSpiniesEv
int daJgm_c::CountSpinies()
{
    int count = 0;
    dActor_c *spiny = 0;
    do {
        spiny = dActor_c::FindWithActorID(0x104, spiny);
        if (spiny)
            count++;
    } while (spiny);
    return count;
}

// @symbol _ZN7daJgm_c14UpdateHoverBobEv
void daJgm_c::UpdateHoverBob()
{
    unsigned short v = (unsigned short)mBobAngle;
    short s = data_02082214[(v >> 4) * 2];
    long long prod = (long long)s * 0x27000;
    mBobOffsetY = (int)((prod + 0x800) >> 12);
    mBobAngle = (short)(mBobAngle + 0x700);
}

// @symbol _ZN7daJgm_c16OnAimedAtWithEggEv
int daJgm_c::OnAimedAtWithEgg()
{
    return 245760;
}

/* Static-init globals (was the handwritten __sinit_ov077_02127240 shard).
 * Definition order is the retail initializer's construction order: the three
 * model handles (files 0x34c/0x429/0x42a), the two animation handles (files
 * 0x34f/0x34d), the two texture-sequence handles (files 0x350/0x34e), then
 * the five-pair state table copied from the ROM PMF constants, then the
 * collision-offset vector. */
DaJgmModelFile data_ov077_02127b50(ASSET_JUGEM_BMD);
DaJgmModelFile data_ov077_02127b48(ASSET_PAIPO_BMD);
DaJgmModelFile data_ov077_02127b38(ASSET_TOGEZO_BMD);
DaJgmAnimationFilePtr data_ov077_02127b28(ASSET_JUGEM_WAIT_BCA);
DaJgmAnimationFilePtr data_ov077_02127b40(ASSET_JUGEM_THROW_BCA);
DaJgmTexSequenceFilePtr data_ov077_02127b20(ASSET_JUGEM_WAIT_BTP);
DaJgmTexSequenceFilePtr data_ov077_02127b30(ASSET_JUGEM_THROW_BTP);
DaJgmStateHandlers data_ov077_02127bc4[5] = {
    {data_ov077_021277e0, data_ov077_02127828},
    {data_ov077_02127820, data_ov077_021277e8},
    {data_ov077_02127818, data_ov077_02127800},
    {data_ov077_021277f0, data_ov077_02127808},
    {data_ov077_02127810, data_ov077_021277f8}
};
JgmVector3 data_ov077_02127b88(0, -0x1e000, 0);

// @symbol _ZN7daJgm_c13OnYoshiTryEatEv
int daJgm_c::OnYoshiTryEat()
{
    return 6;
}
