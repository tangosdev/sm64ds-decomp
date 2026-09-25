//cpp
/**
 * Ice block (profile ICE_BLOCK_LL), with a Power Star frozen inside.
 *
 * Fire melts it: the block shrinks over 30 frames under a pair of recycled
 * particle systems and is gone. An explosion or a mega character shatters it
 * instead (Kill). Either way the star is let go through
 * func_ov081_02127be0, which destroys the frozen daStar_c (actor 0xb2) and
 * respawns it free-standing, with a daStarBase_c (actor 0xb4) beside it.
 *
 * daObjIceBlock_c_classInit is reconstructed (RTTI daObjIceBlock_c,
 * ICE_BLOCK_LL registry). Retail does not store that spelling.
 * Historical alias: IceBlock_Spawn.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 *
 * Known limits:
 * - dBgW_KcMbg::SetFile, dCcAc_c::Init, Particle::System::New / NewSimple
 *   and dBgActor_c::IsClsnInRangeOnScreen stay mangled: each takes
 *   Fix12<int> by value (notes/mwccarm-codegen.md 6az). cstd::fdiv stays
 *   mangled because no header declares the cstd namespace yet.
 * - data_ov081_02128fd0 / 02128fd8 are this overlay's KCL / BMD file
 *   handles and data_ov002_0210d8d4 the CLPS block, all in data this TU
 *   does not own.
 * - func_ov081_02127be0 keeps its placeholder name, and the particle
 *   system word at +0x44 has no member in Particle::System yet.
 */

#include "daObjIceBlock_c.h"
#include "Player.h"
#include "PowerStar.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Particle__System.h"

struct KCL_File;
struct CLPS_Block;

extern "C" {
extern SharedFilePtr data_ov081_02128fd0; /* the KCL */
extern SharedFilePtr data_ov081_02128fd8; /* the BMD */
extern CLPS_Block data_ov002_0210d8d4;

u8 DecIfAbove0_Byte(u8 *counter);
void func_02012694(u32 soundID, const Vector3 *pos);
int _ZN4cstd4fdivEii(int num, int den);

void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 effectID, int x, int y, int z);
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 handle, u32 effectID, int x, int y, int z, const Vector3_16f *dir,
    void *callback);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(
    dBgActor_c *self, int a, int b);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *owner, int radius, int height, u32 flags,
    u32 vulnFlags);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);

void func_ov081_02127be0(daObjIceBlock_c *self);
}

enum {
    kStarActorID = 0xb2,
    kStarBaseActorID = 0xb4,
    kMeltFrames = 30,
    kShardHeight = 0x96000, /* 150.0 */
};

// @symbol daObjIceBlock_c_classInit
extern "C" daObjIceBlock_c *daObjIceBlock_c_classInit()
{
    return new daObjIceBlock_c();
}

// @symbol _ZN15daObjIceBlock_c13InitResourcesEv
s32 daObjIceBlock_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov081_02128fd8), 1, 0x17);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCollider, this, 0xc8000, 0x12c000, 0x800002, 0x44000);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov081_02128fd0);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
        &data_ov002_0210d8d4);
    mScale = 0x1000;
    return 1;
}

// @symbol _ZN15daObjIceBlock_c8BehaviorEv
s32 daObjIceBlock_c::Behavior()
{
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0);

    if (mMeltTimer != 0) {
        mScale = _ZN4cstd4fdivEii(mMeltTimer << 12, kMeltFrames << 12);

        if (DecIfAbove0_Byte(&mMeltTimer) == 0) {
            dActor_c *star = mContainedActor;
            if (star != 0 && star->actorID == kStarActorID)
                ((PowerStar *)star)->unk_49f = 0;
            MarkForDestruction();
        } else {
            Vector3 pos;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            pos.y += (int)(((long long)mScale * kShardHeight + 0x800) >> 12);

            mParticleHandle1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mParticleHandle1, 0x77, pos.x, pos.y, pos.z, 0, 0);
            mParticleHandle2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mParticleHandle2, 0x78, pos.x, pos.y, pos.z, 0, 0);

            u32 handle = mParticleHandle1;
            if (handle != 0) {
                Particle::System *sys = Particle::System::FromUniqueID(handle);
                if (sys != 0)
                    *(s32 *)((u8 *)sys + 0x44) = mScale * 0xf;
            }
            handle = mParticleHandle2;
            if (handle != 0) {
                Particle::System *sys = Particle::System::FromUniqueID(handle);
                if (sys != 0)
                    *(s32 *)((u8 *)sys + 0x44) = mScale * 0xf;
            }
        }
        return 1;
    }

    if (mCollider.otherOwner != 0) {
        if (mCollider.hitFlags & 0x40000) {       /* fire: start melting */
            mMeltTimer = kMeltFrames;
            func_02012694(0x7a, (Vector3 *)&mCamSpacePosX);
            func_ov081_02127be0(this);
        }
        if (mCollider.hitFlags & 0x4000)          /* explosion: shatter */
            Kill();
    }

    mCollider.Clear();
    mCollider.Update();
    return 1;
}

// @symbol _ZN15daObjIceBlock_c6RenderEv
s32 daObjIceBlock_c::Render()
{
    Vector3 scale;
    scale.x = mScale;
    scale.y = mScale;
    scale.z = mScale;
    mModel.Render(&scale);
    return 1;
}

// @symbol _ZN15daObjIceBlock_c16CleanupResourcesEv
s32 daObjIceBlock_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov081_02128fd8.Release();
    data_ov081_02128fd0.Release();
    return 1;
}

// @symbol _ZN15daObjIceBlock_c4KillEv
/* vtable slot 31, dBgActor_c's own Kill overridden. Unlike the rest of the
 * family this one plays its sound FIRST, and it throws three shatter
 * particles -- 0x74, 0x75, 0x76 -- at one point 150.0 above itself.
 *
 * The second Vector3 is memberwise on purpose: Vector3 declares a destructor
 * (types.h), so a whole-object assignment compiles to an ldm/stm pair, four
 * instructions where the ROM has six. */
void daObjIceBlock_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    int x = mPosX;
    int y = mPosY + kShardHeight;
    int z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x74, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x75, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x76, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    func_ov081_02127be0(this);
    MarkForDestruction();
}

// @symbol _ZN15daObjIceBlock_c15OnHitByMegaCharER6Player
void daObjIceBlock_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}

// @symbol func_ov081_02127be0
/* Let the frozen star go: destroy it and respawn it free-standing at the
 * same place and angle, with its star base beside it.
 *
 * The position is read through a Vector3 pointer (the ROM forms star + 0x5c
 * once and loads off it), and the angle is one Vector3_16 struct copy -- three
 * ldrh/strh pairs, where member-by-member s16 reads come out as ldrsh. */
extern "C" void func_ov081_02127be0(daObjIceBlock_c *self)
{
    dActor_c *star = self->mContainedActor;
    if (star == 0)
        return;
    if (star->actorID != kStarActorID)
        return;

    Vector3 pos;
    Vector3 *starPos = (Vector3 *)&star->mPosX;
    u32 param1 = star->param1;
    pos.x = starPos->x;
    pos.y = starPos->y;
    pos.z = starPos->z;

    Vector3_16 rot = *(Vector3_16 *)&self->mContainedActor->mAngleX;

    self->mContainedActor->MarkForDestruction();

    u32 spawnParam = (u8)(param1 & 0xf) | 0x20;
    dActor_c::Spawn(
        kStarBaseActorID, spawnParam, pos, &rot, self->mAreaId, -1);
    self->mContainedActor = dActor_c::Spawn(
        kStarActorID, spawnParam, pos, &rot, self->mAreaId, -1);
    self->mFlags |= 0x4000000;
}

// @symbol _ZN15daObjIceBlock_cD1Ev
// @symbol _ZN15daObjIceBlock_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
