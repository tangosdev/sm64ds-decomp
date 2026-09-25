//cpp
/**
 * Bullet Bill (profile KILLER), fired by daObjBkKillerdai_c.
 *
 * State 0 flies: it hangs in the barrel for 40 frames, shivers for ten,
 * then fires with a smoke trail, homing on the player's yaw while farther
 * than 300.0 away, and blows up after 150 frames or on the first wall that
 * is not its own launcher. State 1 is being shot down: it pitches over and
 * rises for 90 frames before bursting. Behavior runs the state out of a
 * pointer-to-member table and resolves the cylinder's hit: a mega character
 * destroys it outright, a qualifying attack knocks it into state 1, and a
 * player who is not wearing the vanish cap (Player::mIsVanish) takes the
 * hit and does the same.
 *
 * daKlr_c_classInit is reconstructed (RTTI daKlr_c, KILLER registry).
 * Retail does not store that spelling. Historical alias: BulletBill_Spawn.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 *
 * Known limits:
 * - The two state functions func_ov079_0212682c / func_ov079_02126794 are
 *   what the table at data_ov079_021282e0 points at; their member spellings
 *   are not recovered, so they stay free functions taking the actor.
 *   __sinit_ov079_021279d4 fills that table and is not claimed here.
 * - Particle::System::New/NewSimple, dActor_c::DropShadowRadHeight,
 *   dCcAcPos_c::Init, dBgCh_Actr::Init and Player::Hurt take Fix12<int> by
 *   value, so they stay mangled (notes/mwccarm-codegen.md 6az).
 * - data_ov079_02128300 / 021282f0 are the two model file handles, in
 *   overlay data this TU does not own.
 */

#include "daKlr_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Sound.h"
#include "dBgPi.h"

struct BMD_File;

extern "C" {
extern SharedFilePtr data_ov079_02128300;
extern SharedFilePtr data_ov079_021282f0;

void func_0200f760(dActor_c *self, dCc_c *cc);
void func_02012694(u32 id, const Vector3 *pos);
void func_0201267c(u32 id, const Vector3 *pos);
void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);

void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID, int x, int y, int z, void *dir, void *callback);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *self, ShadowModel *shadow, Matrix4x3 *mtx, int radius, int depth,
    u8 opacity);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset, int radius,
    int height, u32 flags, u32 vulnFlags);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, dActor_c *actor, int radius, int height,
    Vector3_16 *a, Vector3_16 *b);
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    Player *self, const Vector3 *source, u32 damage, int speed, u8 a, u8 b,
    u8 c);

void *_ZNK10dBgCh_Actr13GetWallResultEv(const dBgCh_Actr *self);

void func_ov079_02126704(daKlr_c *self);
void func_ov079_02126794(daKlr_c *self);
void func_ov079_0212682c(daKlr_c *self);
}

void ApproachLinear(s16 &value, s16 target, s16 step);

/* The per-state tick table, indexed by mState: [0] func_ov079_0212682c,
   [1] func_ov079_02126794. It lives in .bss and __sinit_ov079_021279d4
   copies the two pointer-to-member constants into it at load. */
typedef void (daKlr_c::*KlrStateFunc)();
extern "C" KlrStateFunc data_ov079_021282e0[];

struct MatrixWords { s32 w[12]; };

enum {
    kPlayerActorID = 0xbf,
    kBurstParticle = 0x8f,
    kSmokeParticle = 0x90,
};

// @symbol daKlr_c_classInit
extern "C" daKlr_c *daKlr_c_classInit()
{
    return new daKlr_c();
}

// @symbol _ZN7daKlr_c13InitResourcesEv
s32 daKlr_c::InitResources()
{
    Model::LoadFile(data_ov079_02128300);
    Model::LoadFile(data_ov079_021282f0);
    if (mModel.SetFile(*(BMD_File **)((char *)&data_ov079_02128300 + 4), 1, -1) == 0)
        return 0;
    if (mModel2.SetFile(*(BMD_File **)((char *)&data_ov079_021282f0 + 4), 1, -1) == 0)
        return 0;
    if (mShadow.InitCylinder() == 0)
        return 0;
    {
        Vector3 offset;
        offset.x = 0;
        offset.y = -0x50000;
        offset.z = 0;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            &mCylClsn, this, &offset, 0x78000, 0xa0000, 0x200005, 0x3c0);
    }
    mState = 0;
    mStateTimer = 0;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWallClsn, this, 0x32000, 0, 0, 0);
    mSmokeTrailID = 0;
    return 1;
}

// @symbol _ZN7daKlr_c8BehaviorEv
s32 daKlr_c::Behavior()
{
    int flags;
    u32 which;
    u32 id;

    func_0200f760(this, &mCylClsn);
    UpdatePos(0);

    which = (u32)mState;
    (this->*data_ov079_021282e0[which])();

    id = mCylClsn.otherOwner;
    if (id != 0) {
        flags = mCylClsn.hitFlags;
        if (flags & 0x10) {
            ((Player *)FindWithID(id))->IncMegaKillCount();
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(kBurstParticle, mPosX, mPosY, mPosZ);
            MarkForDestruction();
            func_02012694(0x78, (Vector3 *)&mCamSpacePosX);
        } else if (flags & 0x3c0) {
            mState = 1;
            Sound::PlayBank0(0xb5, *(Vector3 *)&mCamSpacePosX);
        } else {
            dActor_c *o = FindWithID(id);
            if (o != 0) {
                int isPlayer = o->actorID == kPlayerActorID;
                if (isPlayer) {
                    if (!((Player *)o)->mIsVanish) {
                        Vector3 pos;
                        pos.x = mPosX;
                        pos.y = mPosY;
                        pos.z = mPosZ;
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
                            (Player *)o, &pos, 3, 0xc000, 1, 0, 1);
                        mState = 1;
                        Sound::PlayBank0(0xb5, *(Vector3 *)&mCamSpacePosX);
                    }
                }
            }
        }
    }

    /* The frame counter is bumped through an unsigned view of the field;
       a plain mStateTimer++ misses by one word. */
    {
        u16 *timer = (u16 *)&mStateTimer;
        *timer = *timer + 1;
        if (which != (u32)mState)
            *timer = 0;
    }

    func_ov079_02126704(this);

    mCylClsn.Clear();
    {
        Vector3 offset;
        offset.x = 0;
        offset.y = -0x50000;
        offset.z = 0;
        mCylClsn.SetPosRelativeToActor(offset);
    }
    mCylClsn.Update();

    mPrevAngleX = mAngleX;
    mPrevAngleY = mAngleY;
    mPrevAngleZ = mAngleZ;
    return 1;
}

// @symbol _ZN7daKlr_c6RenderEv
s32 daKlr_c::Render()
{
    mModel.Render(0);
    mModel2.Render(0);
    return 1;
}

// @symbol _ZN7daKlr_c16CleanupResourcesEv
s32 daKlr_c::CleanupResources()
{
    data_ov079_02128300.Release();
    data_ov079_021282f0.Release();
    return 1;
}

// @symbol func_ov079_0212682c
/* State 0: wait in the barrel, shiver, then fly. */
extern "C" void func_ov079_0212682c(daKlr_c *self)
{
    Vector3 nose;
    u16 timer = self->mStateTimer;
    if (timer < 40) {
        self->mHorzSpeed = 0x3000;
        return;
    }
    if (timer < 50) {
        if ((int)timer % 2)
            self->mHorzSpeed = 0x3000;
        else
            self->mHorzSpeed = -0x3000;
        return;
    }
    self->UpdateWMClsn(self->mWallClsn, 0);
    self->mHorzSpeed = -0x1e000;
    self->UpdatePosWithHorzSpeedAndAng();
    Vec3_Add(&nose, (Vector3 *)&self->mPosX, (Vector3 *)&self->unk_0a4);
    self->mSmokeTrailID = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        self->mSmokeTrailID, kSmokeParticle, nose.x, nose.y, nose.z, 0, 0);
    self->mHorzSpeed = 0x1e000;
    if (self->DistToCPlayer() > 0x12c000)
        ApproachLinear(self->mPrevAngleY, self->HorzAngleToCPlayer(), 0x200);
    self->mAngleY = self->mPrevAngleY;
    if ((u16)self->mStateTimer == 50) {
        func_0201267c(0xd4, (Vector3 *)&self->mCamSpacePosX);
        self->mCylClsn.flags &= ~1;
    }
    {
        int hit = 0;
        if ((u16)self->mStateTimer > 150)
            hit = 1;
        if (self->mWallClsn.IsOnWall() != 0) {
            dBgPi *wall = (dBgPi *)_ZNK10dBgCh_Actr13GetWallResultEv(&self->mWallClsn);
            if (wall->GetClsnID() != (u32)-1) {
                if (dActor_c::FindWithID(wall->GetClsnID()) != self->mLauncher)
                    hit = 1;
            } else {
                hit = 1;
            }
        }
        if (hit == 0)
            return;
    }
    if ((u16)self->mStateTimer > 150) {
        func_02012694(0x79, (Vector3 *)&self->mCamSpacePosX);
        self->PoofDust();
    } else {
        func_02012694(0x78, (Vector3 *)&self->mCamSpacePosX);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(kBurstParticle, nose.x, nose.y, nose.z);
    }
    self->MarkForDestruction();
}

// @symbol func_ov079_02126794
/* State 1: shot down -- pitch over and climb, then burst after 90 frames. */
extern "C" void func_ov079_02126794(daKlr_c *self)
{
    if ((u16)self->mStateTimer == 0) {
        self->mHorzSpeed = -0x1e000;
        self->mCylClsn.flags |= 1;
    }
    self->mAngleX += 0x2000;
    self->mPosY += 0x14000;
    if ((u16)self->mStateTimer <= 90)
        return;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(kBurstParticle, self->mPosX, self->mPosY, self->mPosZ);
    func_02012694(0x78, (Vector3 *)&self->mCamSpacePosX);
    self->MarkForDestruction();
}

// @symbol func_ov079_02126704
/* Pose both models from the actor and drop the shadow under them. */
extern "C" void func_ov079_02126704(daKlr_c *self)
{
    Matrix4x3_FromRotationXYZExt(&self->mModel.mat4x3, self->mAngleX, self->mAngleY, self->mAngleZ);
    self->mModel.mat4x3.t.x = self->mPosX >> 3;
    self->mModel.mat4x3.t.y = self->mPosY >> 3;
    self->mModel.mat4x3.t.z = self->mPosZ >> 3;
    /* One 0x30-byte block copy. Matrix4x3's own assignment goes member by
       member through Vector3 and comes out 0x18 bytes longer. */
    *(MatrixWords *)&self->mModel2.mat4x3 = *(MatrixWords *)&self->mModel.mat4x3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, &self->mShadow, &self->mModel.mat4x3, 0x50000, 0x50000, 0xf);
}

// @symbol _ZN7daKlr_c16OnAimedAtWithEggEv
int daKlr_c::OnAimedAtWithEgg()
{
    return 0;
}

// @symbol _ZN7daKlr_cD1Ev
// @symbol _ZN7daKlr_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
