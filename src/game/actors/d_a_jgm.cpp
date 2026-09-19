//cpp
/**
 * Lakitu (`jgm`, JUGEM 265).
 *
 * Hovers on a cloud and throws Spinies (actor 0x104). Five states:
 * hover, throw, yoshi-mouth, spit, fall. ov077 is mixed
 * (HEAVE_HO 238 / SPINY 260 / LAKITU 265). Not daC_Jugem_c
 * (C_JUGEM 235, ov085).
 *
 * daJgm_c_classInit is reconstructed (RTTI daJgm_c, JUGEM registry).
 * Retail does not store that spelling. Historical aliases:
 * Lakitu_Spawn and Lakitu_SpawnInfo.
 *
 * deslop
 * Leftover: factory stays `return new daJgm_c()` (MATCHES 0x60).
 *   Leaf unsigned-long operator new forwards `_ZN7fBase_cnwEj`.
 * Leftover: ordinary .text is reverse source order. Keep the
 *   ROM-high factory first and OnYoshiTryEat last. Inline dtor
 *   in the header emits retail D1 then D0, no D2.
 * Leftover: sizeof wraps on DaJgmStateHandlers / daJgm_c /
 *   DaJgmSpawnInfo / DaJgmVector3Words stay.
 * Leftover: dCcAcPos_c::Init / dBgCh_Actr::Init stay mangled --
 *   Fix12<int> by value (wall 6az). This TU's InitResources.
 *   dBgCh Init header Fix12i mangles as i; ROM is 5Fix12IiE.
 * Leftover: ModelAnim::SetAnim / TextureSequence::SetFile stay
 *   mangled -- Fix12<int> by value (wall 6az). EnterHoverState /
 *   EnterThrowState / EnterFallState.
 * Leftover: dActor_c::DropShadowRadHeight / SpawnCoins stay
 *   mangled -- Fix12<int> by value (wall 6az). UpdateModels /
 *   DieAndDropCoins.
 * Leftover: Player::Bounce / Hurt stay mangled -- Fix12<int> by
 *   value (wall 6az). HandlePlayerCollision. Player.h has neither.
 * Leftover: dActor_c::IsTooFarAwayFromPlayer stays mangled -- not
 *   on dActor_c.h. This TU's Behavior.
 * Leftover: Particle::System::NewSimple stays mangled --
 *   Fix12<int> by value (wall 6az); Particle__System.h has no
 *   NewSimple. EnterFallState.
 * Leftover: dBgCh_Actr_UpdateDiscreteNoLava_veneer is the retail
 *   call (UpdateFallState / UpdateWallAndWater). Named
 *   UpdateDiscreteNoLava is WRONG-DEST (ROM 0x02038420).
 * Leftover: dBgCh_Actr::GetWallResult stays mangled -- not on
 *   dBgCh_Actr.h. UpdateWallAndWater.
 * Leftover: Sound::PlayLong / PlayBank0 stay mangled. PlayLong's
 *   header takes Vector3 & + s16; this TU passes a pointer +
 *   unsigned (UpdateHoverState / UpdateThrowState). PlayBank0
 *   puns mCamSpacePosX (S18); a Vector3 local emits D1 (S3/S23).
 *   EnterFallState / HandlePlayerCollision.
 * Leftover: SharedFilePtr has no recovered fields. SetAnim /
 *   SetFile / Prepare read [1] as the loaded pointer
 *   (EnterHoverState / EnterThrowState / InitResources Prepare).
 * Leftover: data_ov077_02127b50 / 7b48 / 7b38 / 7b28 / 7b40 /
 *   7b20 / 7b30 are overlay BSS SharedFilePtr handles this TU
 *   LoadFile/SetFile/Release. sinit 0x02127240 constructs them
 *   (file IDs 0x34c / 0x429 / 0x42a / 0x34f / 0x34d / 0x350 /
 *   0x34e). Naming belongs with that sinit. This TU claims
 *   .text only.
 * Leftover: data_ov077_02127238[] / 02127230[] are BCA/BTP
 *   pointer tables this TU Init LoadFile / Cleanup Release.
 * Leftover: data_ov077_02127bc4 is the PMF state table SetState
 *   indexes. sinit copies ten ROM PMF constants into it.
 * Leftover: data_ov077_02127b88 is the collision-offset Vector3
 *   InitResources / UpdateHoverState / UpdateThrowState feed
 *   dCcAcPos_c. sinit constructs it.
 * Leftover: data_02082214 is the NitroSDK sin/cos table.
 *   EnterSpitState / UpdateHoverBob index it. Naming belongs
 *   with the SDK table.
 * Leftover: data_020a0e68 is the arm9 scratch Matrix4x3
 *   UpdateModels writes.
 * Leftover: common.h must be first. Nested Matrix.h spelling
 *   scalarizes the 12-word mat4x3 copies; UpdateModels writes
 *   translation as m[9..11].
 * Leftover: DaJgmVector3Words -- a local Vector3 emits
 *   vague-linkage ~Vector3 into this text-only TU (S3/S23).
 *   UpdateHoverState / UpdateThrowState / UpdateWallAndWater /
 *   HandlePlayerCollision / DieAndDropCoins.
 * Leftover: *(Vector3 *)&mPosX / mSpawnPosX -- dActor_c has no
 *   Pos() on this tree (S18). UpdateHoverState / UpdateFlight /
 *   HandlePlayerCollision.
 * Leftover: OnTurnIntoEgg R6Player / UpdateWallAndWater
 *   R10dBgCh_Actr -- a pointer generates identical ARM.
 * Leftover: SetState `int` -- signedness is not distinguishable
 *   from other 32-bit forms.
 * Leftover: func_0201267c throw SFX 0xd2 at +0x74
 *   (mCamSpacePosX). UpdateThrowState. No named method in this
 *   TU.
 * Leftover: ApproachLinear2 is C++ linkage leftover
 *   (_Z15ApproachLinear2Rsss). UpdateSpitState.
 * Leftover: one_arg_setstate so InitResources's SetState call
 *   keeps r1=0 from the caller.
 * Leftover: InitResources / UpdateThrowState keep
 *   `char *c = (char *)this` offset soup and the 64-bit launder
 *   on the spawn-pos stores (named fields CSE).
 * Leftover: S14: g_profile_JUGEM stays outside the licensed
 *   .text (deadstrip-data).
 */

#include "common.h"
#include "daJgm_c.h"
#include "dBgCh_Gnd.h"
#include "Particle__System.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "SurfaceInfo.h"

namespace cstd { int fdiv(int, int); }

/* Local three-word ABI value. Vector3 has a genuine non-trivial destructor;
 * these stack values are plain fixed-point words and must not instantiate its
 * otherwise unrelated vague-linkage D1 in this class TU. */
struct DaJgmVector3Words { Fix12i x, y, z; };

typedef char DaJgmVector3Words_size_must_be_0xc[
    sizeof(DaJgmVector3Words) == 0xc ? 1 : -1];

struct DaJgmSpawnInfo {
    daJgm_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char DaJgmSpawnInfo_size_must_be_0x1c[
    sizeof(DaJgmSpawnInfo) == 0x1c ? 1 : -1];

struct DaJgmMatrixWords { int words[12]; };

extern int _ZTV7daJgm_c[];

extern "C" {
extern SharedFilePtr data_ov077_02127b50;
extern SharedFilePtr data_ov077_02127b48;
extern SharedFilePtr data_ov077_02127b38;
extern SharedFilePtr data_ov077_02127b28;
extern SharedFilePtr data_ov077_02127b40;
extern SharedFilePtr data_ov077_02127b20;
extern SharedFilePtr data_ov077_02127b30;
extern SharedFilePtr *data_ov077_02127238[];
extern SharedFilePtr *data_ov077_02127230[];
extern DaJgmStateHandlers data_ov077_02127bc4[];
extern Vector3 data_ov077_02127b88;
extern s16 data_02082214[];
extern Matrix4x3 data_020a0e68;
extern Matrix4x3 IDENTITY_MATRIX4X3;

extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern void _Z14ApproachLinearRsss(short *a, short b, short c);
extern void _Z14ApproachLinearRiii(int *a, int b, int c);
extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *collision);
extern void *_ZNK10dBgCh_Actr13GetWallResultEv(dBgCh_Actr *collision);
extern void func_0201267c(int id, void *pos);

extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 angY);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void *a, void *b, void *out);
extern void Vec3_LslInPlace(void *v, int sh);
extern void Vec3_Asr(void *d, void *s, int sh);

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern int _ZN9Animation8FinishedEv(void *self);
extern void _ZN9Animation7AdvanceEv(void *self);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *self, Vector3 *v);
extern void _ZN5dCc_c5ClearEv(void *self);
extern void _ZN5dCc_c6UpdateEv(void *self);
extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    unsigned int, unsigned int, Vector3 *, void *, int, int);
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(dActor_c *, int);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *model, void *file, int flags, int speed, unsigned int startFrame);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    TextureSequence *seq, void *file, int flags, int speed, unsigned int startFrame);
extern int _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *collision, dActor_c *actor, const Vector3 *offset,
    Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags);
extern int _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *collision, dActor_c *actor, Fix12i radius, Fix12i height,
    void *a, void *b);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix,
    Fix12i radius, Fix12i depth, u32 opacity);
extern void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(
    dActor_c *actor, const DaJgmVector3Words *pos, unsigned int count,
    int spread, short angle);
extern void _ZN6Player6BounceE5Fix12IiE(Player *player, int vel);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    Player *player, const DaJgmVector3Words *pos, unsigned int a, int vel,
    unsigned int b, unsigned int c, unsigned int d);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(
    unsigned int handle, unsigned int a, unsigned int id,
    const Vector3 *pos, unsigned int e);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int n, const Vector3 &v);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    unsigned int n, int x, int y, int z);
}

/* Leftover C++ linkage: mangles as _Z15ApproachLinear2Rsss. */
void ApproachLinear2(short &v, short t, short step);

namespace one_arg_setstate {
extern "C" void _ZN7daJgm_c8SetStateEi(void *self);
}

// @symbol daJgm_c_classInit
extern "C" daJgm_c *daJgm_c_classInit()
{
    return new daJgm_c();
}

extern "C" DaJgmSpawnInfo g_profile_JUGEM = {
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
    char *c = (char *)this;
    _ZN5Model8LoadFileER13SharedFilePtr((void *)&data_ov077_02127b38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(
        c + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr((void *)&data_ov077_02127b50), 1, 1);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(
        c + 0x138,
        _ZN5Model8LoadFileER13SharedFilePtr((void *)&data_ov077_02127b48), 1, 1);
    for (int i = 0; i < 2; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr((void *)data_ov077_02127238[i]);
    for (int i = 0; i < 2; i++) {
        void *t = (void *)data_ov077_02127230[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(t);
        TextureSequence::Prepare(
            *(BMD_File *)((int *)&data_ov077_02127b50)[1],
            *(BTP_File *)((int *)t)[1]);
    }
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0)
        return 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        (dCcAcPos_c *)(c + 0x1c4), (dActor_c *)c, (Vector3 *)&data_ov077_02127b88,
        0x41000, 0x78000, 0x200002, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        (dBgCh_Actr *)(c + 0x204), (dActor_c *)c, 0x2d000, 0x2d000, 0, 0);

    *(int *)(((int)(c + 0x9c)) & 0xFFFFFFFFFFFFFFFFLL) = 0;
    *(int *)(((int)(c + 0xa0)) & 0xFFFFFFFFFFFFFFFFLL) = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    *(int *)(((int)(c + 0x3f8)) & 0xFFFFFFFFFFFFFFFFLL) =
        *(int *)(((int)(c + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
    *(int *)(((int)(c + 0x3fc)) & 0xFFFFFFFFFFFFFFFFLL) =
        *(int *)(((int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);
    mSpawnPosZ = *(int *)(((int)(c + 0x64)) & 0xFFFFFFFFFFFFFFFFLL);
    *(int *)(((int)(c + 0x410)) & 0xFFFFFFFFFFFFFFFFLL) = 0;

    one_arg_setstate::_ZN7daJgm_c8SetStateEi(c);
    *(Matrix4x3 *)((char *)&mMatrix) = IDENTITY_MATRIX4X3;
    UpdateModels();
    return 1;
}

// @symbol _ZN7daJgm_c8BehaviorEv
s32 daJgm_c::Behavior()
{
    int v = param1;
    if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(
            this, v ? 0x1068000 : 0x7d0000))
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
    mTextureSequence.Update(mModelAnim.data);
    mModelAnim.Render(0);
    if (mState == 1) {
        unsigned int v = ((unsigned int)(mModelAnim.currFrame << 4)) >> 0x10;
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

    int i;
    for (i = 0; i < 2; i++)
        data_ov077_02127238[i]->Release();

    for (i = 0; i < 2; i++)
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
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, *(void **)((char *)&data_ov077_02127b28 + 4),
        0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &mTextureSequence, *(void **)((char *)&data_ov077_02127b20 + 4),
        0, 0x1000, 0);
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
        Player *p = ClosestNonVanishPlayer();
        if (p == 0)
            goto store2;
        if (p->mPosY >= mPosY)
            goto store1;
        if (Vec3_HorzDist((Vector3 *)&mPosX, (Vector3 *)&p->mPosX) >= 0x190000)
            goto store1;
        if (CountSpinies() >= 4)
            goto store1;
        SetState(1);
        goto tail;
    store1:
        mTimer = 0x96;
        goto tail;
    store2:
        mTimer = 0x96;
    }
tail:
    UpdateFlight();
    UpdateHoverBob();
    mModelAnim.Advance();
    mTextureSequence.Advance();
    HandlePlayerCollision();
    {
        DaJgmVector3Words pos;
        pos.y = data_ov077_02127b88.y + mBobOffsetY;
        pos.x = data_ov077_02127b88.x;
        pos.z = data_ov077_02127b88.z;
        mdCcAcPos_c.SetPosRelativeToActor(*(Vector3 *)&pos);
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        mSoundHandle, 3, 0x182, (const Vector3 *)&mCamSpacePosX, 0);
    return 1;
}

// @symbol _ZN7daJgm_c15EnterThrowStateEv
s32 daJgm_c::EnterThrowState()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, *(void **)((char *)&data_ov077_02127b40 + 4),
        0, 0x1000, 0);
    mModelAnim.SetFlags(0x40000000);
    mModelAnim.speed = 0x1000;
    mModelAnim.currFrame = 0;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &mTextureSequence, *(void **)((char *)&data_ov077_02127b30 + 4),
        0, 0x1000, 0);
    mTextureSequence.SetFlags(0x40000000);
    mTextureSequence.speed = 0x1000;
    mTextureSequence.currFrame = 0;
    mState = 1;
    return 1;
}

// @symbol _ZN7daJgm_c16UpdateThrowStateEv
s32 daJgm_c::UpdateThrowState()
{
    char *c = (char *)this;
    if ((((unsigned int)*(int *)(c + 0x12c) << 4) >> 16) == 0x3a) {
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            0x104, 0, (Vector3 *)(c + 0x404), (void *)(c + 0x8c),
            *(signed char *)(c + 0xcc), -1);
        func_0201267c(0xd2, c + 0x74);
    }
    if (_ZN9Animation8FinishedEv(c + 0x124))
        SetState(0);
    UpdateFlight();
    UpdateHoverBob();
    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN9Animation7AdvanceEv(c + 0x1b0);
    HandlePlayerCollision();
    {
        DaJgmVector3Words pos;
        pos.y = data_ov077_02127b88.y + mBobOffsetY;
        pos.x = data_ov077_02127b88.x;
        pos.z = data_ov077_02127b88.z;
        mdCcAcPos_c.SetPosRelativeToActor(*(Vector3 *)&pos);
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        mSoundHandle, 3, 0x182, (const Vector3 *)&mCamSpacePosX, 0);
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
        char *p = (char *)mCarryActor;
        int *src = (int *)(p + 0x5c);
        mPosX = src[0];
        mPosY = src[1];
        mPosZ = src[2];
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
    dActor_c *d0;
    int *a5c;
    int *a60;
    int *a64;
    int *src;
    int k;
    s16 s;
    s16 cval;

    mFlags &= ~0x80000;

    d0 = mCarryActor;
    a5c = &mPosX;
    mHorzSpeed = d0->mHorzSpeed + 0xa000;

    d0 = mCarryActor;
    a60 = &mPosY;
    a64 = &mPosZ;
    mPrevAngleY = d0->mAngleY;

    d0 = mCarryActor;

    src = &d0->mPosX;
    mPosX = src[0];
    mPosY = src[1];
    mPosZ = src[2];

    k = ((int)(u16)mPrevAngleY) >> 4;
    s = data_02082214[k * 2];
    *a5c = *a5c + (int)(((s64)s * 0x50000 + 0x800) >> 12);
    *a60 = *a60 + 0x50000;
    k = ((int)(u16)mPrevAngleY) >> 4;
    cval = data_02082214[k * 2 + 1];
    *a64 = *a64 + (int)(((s64)cval * 0x50000 + 0x800) >> 12);

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
    _ZN5Sound9PlayBank0EjRK7Vector3(9, *(Vector3 *)&mCamSpacePosX);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mTimer = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, *(void **)((char *)&data_ov077_02127b28 + 4),
        0, 0x1000, 0);
    mModelAnim.speed = 0x4000;
    {
        int r1 = OnAimedAtWithEgg();
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
            0x43, mPosX, mPosY + r1, mPosZ);
        int r2 = OnAimedAtWithEgg();
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
            0x44, mPosX, mPosY + r2, mPosZ);
    }
    mState = 4;
    return 1;
}

// @symbol _ZN7daJgm_c15UpdateFallStateEv
s32 daJgm_c::UpdateFallState()
{
    mAngleX = (short)(mAngleX - 0x1000);
    mModelAnim.Advance();
    UpdatePos(&mdCcAcPos_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    if (!mWithMeshClsn.JustHitGround()) {
        if (DecIfAbove0_Byte(&mTimer))
            goto end;
    }
    DieAndDropCoins();
end:
    return 1;
}

// @symbol _ZN7daJgm_c12UpdateModelsEv
void daJgm_c::UpdateModels()
{
    int b = (int)((mFlags & 0x40000) != 0);
    if (b)
        return;

    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.m[9] = mPosX >> 3;
    mModelAnim.mat4x3.m[10] = (mPosY + mBobOffsetY) >> 3;
    mModelAnim.mat4x3.m[11] = mPosZ >> 3;

    if (mState == 4) {
        data_020a0e68 = mModelAnim.mat4x3;
        int y1 = OnAimedAtWithEgg() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
        int y2 = (-OnAimedAtWithEgg()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);
        mModelAnim.mat4x3 = data_020a0e68;
    }

    mMatrix.m[9] = mPosX >> 3;
    mMatrix.m[10] = mPosY >> 3;
    mMatrix.m[11] = mPosZ >> 3;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mMatrix, 0x5a000, 0x320000, 0xf);

    if (mState != 1)
        return;

    u32 t = (u32)(mModelAnim.currFrame << 4) >> 0x10;
    if (t < 0x19)
        return;
    if (t > 0x3a)
        return;

    struct { int t[3]; int v[3]; } lv;
    lv.t[0] = -0x2000;
    lv.t[1] = 0x1800;
    lv.t[2] = 0x1000;

    data_020a0e68 = mModelAnim.mat4x3;
    MulMat4x3Mat4x3(
        (char *)mModelAnim.data.transforms + 0x90, &data_020a0e68,
        &data_020a0e68);

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
        SurfaceInfo *info = (SurfaceInfo *)(
            (char *)_ZNK10dBgCh_Actr13GetWallResultEv(&collision) + 4);
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
    unsigned char *r4;
    int b;

    if (FindEgg(mdCcAcPos_c) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, *(const Vector3 *)&mCamSpacePosX);
        DieAndDropCoins();
        return;
    }

    {
        unsigned int id = mdCcAcPos_c.otherOwner;
        if (id == 0)
            return;
        r4 = (unsigned char *)dActor_c::FindWithID(id);
    }
    if (r4 == 0)
        return;

    b = (int)(((dActor_c *)r4)->actorID == 0xbf);
    if (b == 0)
        return;

    b = (int)((mFlags & 0x20000) != 0);
    if (b != 0) {
        SetState(2);
        return;
    }

    Player *player = (Player *)r4;
    if ((mdCcAcPos_c.hitFlags & 0x66fe0)
        || BumpedUnderneathByPlayer(*player) != 0
        || player->IsOnShell() != 0
        || player->mIsMetal != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, *(const Vector3 *)&mCamSpacePosX);
        DieAndDropCoins();
        return;
    }

    if (mdCcAcPos_c.hitFlags & 0x10) {
        mPrevAngleY = Vec3_HorzAngle((Vector3 *)(r4 + 0x5c), (Vector3 *)&mPosX);
        mAngleY = (short)(mPrevAngleY + 0x8000);
        player->IncMegaKillCount();
        SetState(4);
        return;
    }

    if (JumpedOnByPlayer(mdCcAcPos_c, *player) != 0) {
        _ZN6Player6BounceE5Fix12IiE(player, 0x28000);
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
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
            player, &v, 2, 0xc000, 1, 0, 1);
    }
}

// @symbol _ZN7daJgm_c15DieAndDropCoinsEv
void daJgm_c::DieAndDropCoins()
{
    DaJgmVector3Words t;
    t.x = mPosX;
    t.y = mPosY;
    t.z = mPosZ;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, &t, 5, 0x3000, 0);
    PoofDust();
    KillAndTrackInDeathTable();
}

// @symbol _ZN7daJgm_c12UpdateFlightEv
void daJgm_c::UpdateFlight()
{
    Player *p = ClosestNonVanishPlayer();
    char *tgt;
    int r6;
    if (p != 0) {
        r6 = (param1 != 0) ? 0x1068000 : 0x7d0000;
        if (Vec3_HorzDist((Vector3 *)&mSpawnPosX, (Vector3 *)&p->mPosX) < r6) {
            int d = mSpawnPosY - p->mPosY;
            if (d < 0)
                d = -d;
            if (d < 0x5dc000) {
                tgt = (char *)&p->mPosX;
                goto L64;
            }
        }
        tgt = (char *)&mSpawnPosX;
    } else {
        tgt = (char *)&mSpawnPosX;
    }
L64:
    {
        int hd = Vec3_HorzDist((Vector3 *)&mPosX, (Vector3 *)tgt);
        short ha = Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)tgt);
        _Z14ApproachLinearRsss(&mAngleY, ha, 0x5e8);
        if (mTurnDir != 0)
            mPrevAngleY = (short)(mAngleY - TurnOffsetFromDist(hd));
        else
            mPrevAngleY = (short)(mAngleY + TurnOffsetFromDist(hd));
        _Z14ApproachLinearRiii(&mPosY, *(int *)(tgt + 4) + 0x12c000, 0x2000);
        UpdatePos(&mdCcAcPos_c);
        UpdateWallAndWater(mWithMeshClsn);
    }
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
    int r5 = 0;
    dActor_c *r1 = 0;
    do {
        r1 = dActor_c::FindWithActorID(0x104, r1);
        if (r1)
            r5++;
    } while (r1);
    return r5;
}

// @symbol _ZN7daJgm_c14UpdateHoverBobEv
void daJgm_c::UpdateHoverBob()
{
    unsigned short v = (unsigned short)mBobAngle;
    short s = *(short *)((char *)data_02082214 + ((v >> 4) << 2));
    long long prod = (long long)s * 0x27000;
    mBobOffsetY = (int)((prod + 0x800) >> 12);
    mBobAngle = (short)(mBobAngle + 0x700);
}

// @symbol _ZN7daJgm_c16OnAimedAtWithEggEv
int daJgm_c::OnAimedAtWithEgg()
{
    return 245760;
}

// @symbol _ZN7daJgm_c13OnYoshiTryEatEv
int daJgm_c::OnYoshiTryEat()
{
    return 6;
}
