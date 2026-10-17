//cpp
/* The snowman's breath. Fifty SnowmanBreathParticle members sit in this
 * file because their functions sit between the actor's destructor and its
 * other methods. .text 0x021120c4..0x02112b14.
 *
 * _ZTS 0x02113b1c, _ZTI 0x02113b10 (base _ZTI8dActor_c 0x0208e390),
 * _ZTV 0x02113b50. Was SnowmanBreath. Key function ~daSnmBth_c under
 * defer_codegen off: D1 0x021120c4, D0 0x02112104, D2 deadstripped.
 */
// leftovers:
// - Particle::System::New and NewSimple, Sound::PlayLong, dCcPos_c::Init,
//   Player::BlowAway and HasFinishedTalking stay mangled. This TU passes
//   Fix12<int> by value.
// - SnowmanBreathParticle is a coined name. ov027 has no type string for
//   the member struct.
// - mInvModelMat stays flat words. A Matrix4x3 member reorders the
//   destructor group to D2, D0, D1.
// - IsPlayerInRange tests the box as one positive `&&` condition. The
//   negated `||` form makes the function 4 bytes larger.

#pragma defer_codegen off

#include "daSnmBth_c.h"
#include "decl_common.h"
#include "decl_Player.h"
#include "SharedFilePtr.h"
#include "types.h"
#include "Model.h"
#include "Player.h"
#include "SaveData.h"

/* This scene owns the position used by the breath effect. */
extern Vector3 data_ov027_02113d10;

extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32, s32, s32, s32);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32, u32, s32, s32, s32, const void *, void *);
s16 Vec3_HorzAngle(const Vector3 *, const Vector3 *);
s32 Vec3_Dist(const Vector3 *, const Vector3 *);
void Matrix4x3_FromRotationY(Matrix4x3 *, s16);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *, s16);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *, s32, s32, s32);
void AddVec3(const Vector3 *, const Vector3 *, Vector3 *);
int DecIfAbove0_Byte(void *);
s32 RandomIntInternal(s32 *);
/* Fix12<int> by-value arguments are kept in their measured raw ABI view: a
 * direct C++ definition of this imported signature hits the mwccarm 6az wall. */
void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
    dCcPos_c *, const Vector3 *, s32, s32, u32, u32);
void Vec3_Asr(Vector3 *, const Vector3 *, int);
void MulVec3Mat4x3(const Vector3 *, const Matrix4x3 *, Vector3 *);
void Vec3_LslInPlace(Vector3 *, int);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, short angY);
void InvMat4x3(Matrix4x3 *dst, const Matrix4x3 *src);
/* The Sound call below stays a mangled name inside this block: it takes
 * its position as void*, and a bare `extern` on a mangled name in a C++ file
 * is mangled a second time (ShowMessage once reached the linker as
 * _Z48_ZN6Player11ShowMessage...). Byte gates cannot see that, because
 * relocations compare as wildcards; only the link does. */
int _ZN5Sound8PlayLongEjjjRK7Vector3s(int handle, unsigned int a,
                                     unsigned int b, void *pos,
                                     unsigned int c);
extern Matrix4x3 data_020a0e68;
extern s32 data_0209e650;
extern u8 data_0209f2d8[];
extern SharedFilePtr data_ov002_0210da40;
extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9c0;
/* Whole-matrix copies (InitResources, IsPlayerInRange) cast through a plain
 * 48-byte struct; that is the form both were matched in, so it stays. */
typedef struct { s32 words[12]; } MatrixWords;
}

/* Actor ID 0xbf is PLAYER. int-returning, so the ROM materializes the 0/1
 * before testing it; a direct `!=` does not. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

// @symbol _ZN10daSnmBth_cD1Ev
// @symbol _ZN10daSnmBth_cD0Ev
daSnmBth_c::~daSnmBth_c()
{
}

// @symbol _ZN21SnowmanBreathParticleD1Ev
SnowmanBreathParticle::~SnowmanBreathParticle()
{
}

// @symbol _ZN21SnowmanBreathParticle9HitPlayerEv
void SnowmanBreathParticle::HitPlayer()
{
    u32 id = mCollider.otherOwner;
    if (id == 0)
        return;
    dActor_c *actor = dActor_c::FindWithID(id);
    if (actor == 0)
        return;
    if (!IsPlayerActor(actor))
        return;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x100, mPos.x, mPos.y, mPos.z);

    Player *player = (Player *)actor;
    if (player->mIsMetal)
        return;
    if (player->mIsMega)
        return;
    if (player->mIsBalloon)
        return;
    _ZN6Player8BlowAwayEs(player, mAngleY);
    if (player->IsCollectingCap())
        return;
    if (player->mHasWings)
        return;
    if (player->mIsVanish)
        return;

    s32 angle = Vec3_HorzAngle(&data_ov027_02113d10, &actor->mPos);
    Vector3_16 rotation;
    rotation.x = 0;
    rotation.y = (s16)angle;
    rotation.z = 0;
    if (player->mCharacter != actor->param1) {
        player->SetNewHatCharacter(player->mCharacter, 0, 0);
    } else {
        if (SaveData::HasPlayerLostCap())
            return;
        SaveData::PlayerLoseCap();
    }

    Vector3 spawnPos;
    {
        s32 posX = mPos.x;
        s32 posZ = mPos.z;
        s32 posY = mPos.y + 0x96000;
        spawnPos.x = posX;
        spawnPos.y = posY;
        spawnPos.z = posZ;
    }
    dActor_c *cap = dActor_c::Spawn(
        0x10d, (actor->param1 << 8) | 3, spawnPos, &rotation, 0, -1);
    if (cap == 0)
        return;
    cap->unk_0a4 = 0;
    cap->mVertSpeed = 0x14000;
    cap->unk_0ac = 0;
    cap->mHorzSpeed = 0x1c000;
}

// @symbol _ZN21SnowmanBreathParticle12CheckSnowmanEv
void SnowmanBreathParticle::CheckSnowman()
{
    dActor_c *snowman = dActor_c::FindWithActorID(0x102, 0);
    s32 threshold = mCollider.radius + 0x12b000;
    s32 distance = Vec3_Dist(&mPos, &snowman->mPos);
    if (distance >= threshold)
        return;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x100, mPos.x, mPos.y, mPos.z);
    mTimer = 0;
}

// @symbol _ZN21SnowmanBreathParticle14UpdatePositionEv
void SnowmanBreathParticle::UpdatePosition()
{
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0, 0x50000);
    Vector3 offset;
    offset = data_020a0e68.t;
    AddVec3(&mPos, &offset, &mPos);
}

// @symbol _ZN21SnowmanBreathParticle6RenderEv
void SnowmanBreathParticle::Render()
{
    if (mTimer == 0)
        return;
    mParticleID =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            mParticleID, 0x111, mPos.x, mPos.y, mPos.z, 0, 0);
}

// @symbol _ZN21SnowmanBreathParticle8BehaviorEv
void SnowmanBreathParticle::Behavior()
{
    if (DecIfAbove0_Byte(&mTimer) == 0)
        return;
    UpdatePosition();
    CheckSnowman();
    HitPlayer();
    mCollider.pos = mPos;
    mCollider.Clear();
    mCollider.Update();
}

// @symbol _ZN21SnowmanBreathParticle8TrySpawnER6Player
int SnowmanBreathParticle::TrySpawn(Player &player)
{
    s32 angle;
    if (mTimer != 0)
        return 0;
    mPos = data_ov027_02113d10;
    angle = Vec3_HorzAngle(&mPos, &player.mPos);
    if (angle > 0x2af8)
        angle = 0x2af8;
    if (angle < 0x9de)
        angle = 0x9de;
    {
        s32 random = RandomIntInternal(&data_0209e650);
        mAngleX = 0xc1c;
        mAngleY = (s16)(angle + ((random & 0x1fff) - 0x1000));
    }
    mAngleZ = 0;
    mTimer = 0x16;
    return 1;
}

// @symbol _ZN21SnowmanBreathParticleC1Ev
SnowmanBreathParticle::SnowmanBreathParticle()
{
    mPos = data_ov027_02113d10;
    _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
        &mCollider, &mPos, 0x32000, 0x28000, 0x800002, 0x800000);
    mTimer = 0;
}

// @symbol _ZN10daSnmBth_c15IsPlayerInRangeEv
int daSnmBth_c::IsPlayerInRange()
{
    Player *player = ClosestPlayer();
    mTalkPlayer = player;

    Vector3 playerPos;
    Vec3_Asr(&playerPos, &player->mPos, 3);
    *(MatrixWords *)&data_020a0e68 = *(MatrixWords *)&mInvModelMat;

    Vector3 localPos;
    MulVec3Mat4x3(&playerPos, &data_020a0e68, &localPos);
    Vec3_LslInPlace(&localPos, 3);

    if (localPos.x >= -0xe1000 && localPos.x <= 0xe1000 && localPos.y >= -0x5000 &&
        localPos.y <= 0x300000 && localPos.z >= -0x430000 && localPos.z <= 0x430000)
        return 1;
    return 0;
}

// @symbol _ZN10daSnmBth_c16CleanupResourcesEv
/* daSnmBth_c::CleanupResources -- vtable slot 3. Releases the three shared
 * files the class holds; it never touches `this`. */
int daSnmBth_c::CleanupResources()
{
    data_ov002_0210da40.Release();
    data_ov002_0210d9a0.Release();
    data_ov002_0210d9c0.Release();
    return 1;
}

// @symbol _ZN10daSnmBth_c16OnPendingDestroyEv
/* daSnmBth_c::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void daSnmBth_c::OnPendingDestroy()
{
}

// @symbol _ZN10daSnmBth_c6RenderEv
int daSnmBth_c::Render()
{
  int b = data_0209f2d8[0];
  b = (b == 2);
  if(b) return 1;
  {
    int i;
    for(i=0;i<0x32;i++){
      mParticles[i].Render();
    }
  }
  return 1;
}

// @symbol _ZN10daSnmBth_c8BehaviorEv
int daSnmBth_c::Behavior()
{
    int b;
    SnowmanBreathParticle *slotBase;
    u8 *idx;
    Vector3 pos;
    int i;
    SnowmanBreathParticle *slot;
    int zero;

    b = data_0209f2d8[0];
    b = (b == 2);
    if (b) {
        return 1;
    }

    if (mTalkDone == 0) {
        switch (mTalkState) {
        case 0:
            if (IsPlayerInRange() == 0) {
                break;
            }
            if (mTalkPlayer->StartTalk(*this, 1) == 0) {
                break;
            }
            mTalkState++;
            break;
        case 1:
            pos.x = mPosX;
            zero = 0;
            pos.y = mPosY;
            pos.z = mPosZ;
            pos.y = pos.y + 0x12c000;
            if (mTalkPlayer->ShowMessage(
                    *this, 0xbb, &pos, zero, zero) == 0) {
                break;
            }
            mTalkState++;
            break;
        case 2:
            if (mTalkPlayer->GetTalkState() == -1) {
                _ZN6Player18HasFinishedTalkingEv(mTalkPlayer);
                mTalkDone = 1;
            }
            break;
        }
    } else {
        if (IsPlayerInRange() != 0) {
            mTalkTimer++;
            if ((mTalkTimer & 7) != 0) {
                slotBase = mParticles;
                idx = &mNextParticle;
                do {
                    slot = slotBase + mNextParticle;
                    b = slot->TrySpawn(*mTalkPlayer);
                    *idx = *idx + 1;
                    *idx = *idx % 0x32;
                } while (b == 0);
            }
            mSoundHandle =
                _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundHandle, 3, 0x184,
                                                  &mCamSpacePosX, 0);
        }
    }

    slot = mParticles;
    i = 0;
    do {
        slot->Behavior();
        i++;
        slot++;
    } while (i < 0x32);

    return 1;
}

// @symbol _ZN10daSnmBth_c13InitResourcesEv
int daSnmBth_c::InitResources()
{
    Model::LoadFile(data_ov002_0210da40);
    Model::LoadFile(data_ov002_0210d9a0);
    Model::LoadFile(data_ov002_0210d9c0);
    mPosX = 0x3fa770;
    mPosY = 0xcb2000;
    mPosZ = 0x46988e;
    mAngleY = 0x5d30;
    Vector3 pos;
    Vec3_Asr(&pos, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos.x, pos.y, pos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    InvMat4x3(&data_020a0e68, &data_020a0e68);
    *(MatrixWords *)&mInvModelMat = *(MatrixWords *)&data_020a0e68;
    return 1;
}

/* SNOWMAN_BREATH factory. */
// @symbol daSnmBth_c_classInit
extern "C" daSnmBth_c *daSnmBth_c_classInit()
{
    return new daSnmBth_c();
}

/* Construction and destructor registration belong to this translation unit. */
Vector3 data_ov027_02113d10(0x27a52b, 0xdf2000, 0x2626bb);
