//cpp
/**
 * Ice block (profile ICE_BLOCK_LL), with a Power Star frozen inside.
 *
 * Fire melts it: the block shrinks over 30 frames under a pair of recycled
 * particle systems and is gone. An explosion or a mega character shatters it
 * instead (Kill). Either way the star is let go through
 * ReleaseStar, which destroys the frozen daStar_c (actor 0xb2) and
 * respawns it free-standing, with a daStarBase_c (actor 0xb4) beside it.
 *
 * daObjIceBlock_c_classInit is reconstructed (RTTI daObjIceBlock_c,
 * ICE_BLOCK_LL registry). Retail does not store that spelling.
 * Historical alias: IceBlock_Spawn.
 *
 * _ZTS15daObjIceBlock_c 0x02128c8c. _ZTI15daObjIceBlock_c 0x02128c80.
 * _ZTV15daObjIceBlock_c 0x02128cc4. InitResources is the key function,
 * first non-inline virtual: D1 at 0x02127b34, D0 at 0x02127b80.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 *
 * Notes:
 * - data_ov002_0210d8d4 is the CLPS block, in ov002 data this TU does not
 *   own.
 * - The particle system word at +0x44 has no member in Particle::System yet.
 * - daStar_c's unk_49f (written when the star is let go) lives on another
 *   class's header, which has not named it yet.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_ice_block_AssetId { ASSET_ICE_BLOCK_LL_BMD = 0x045a, ASSET_ICE_BLOCK_LL_KCL = 0x045b };

#include "daObjIceBlock_c.h"
#include "Player.h"
#include "daStar_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Particle__System.h"
#include "nitro/fx/fx.h"
#include "cstd.h"
#include "math/CalcLib.h"

struct KCL_File;
struct CLPS_Block;

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct IceBlockModelFilePtr : SharedFilePtr {
    IceBlockModelFilePtr(u32 fileID);
    ~IceBlockModelFilePtr();
};

struct IceBlockCollisionFilePtr : SharedFilePtr {
    IceBlockCollisionFilePtr(u32 fileID);
    ~IceBlockCollisionFilePtr();
};

/* Defined at the end of this file so the constructors do not enter .text. */
extern IceBlockCollisionFilePtr data_ov081_02128fd0; /* the KCL */
extern IceBlockModelFilePtr data_ov081_02128fd8;     /* the BMD */
extern CLPS_Block data_ov002_0210d8d4;

extern "C" {
/* local extern: arm9 sound helper, no header declares it. */
void func_02012694(u32 soundID, const Vector3 *pos);
}

enum {
    kStarActorID = 0xb2,
    kStarBaseActorID = 0xb4,
    kMeltFrames = 30,
    kShardHeight = 0x96000, /* 150.0 */
};

// @symbol daObjIceBlock_c_classInit
extern "C" daObjIceBlock_c *daObjIceBlock_c_classInit() {
    return new daObjIceBlock_c();
}

// @symbol _ZN15daObjIceBlock_c13InitResourcesEv
s32 daObjIceBlock_c::InitResources() {
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov081_02128fd8), 1, 0x17);
    mCollider.Init(this, 0xc8000, 0x12c000, 0x800002, 0x44000);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov081_02128fd0);
    ((dBgW_KcMbg *)(&mMeshCollider))
        ->dBgW_KcMbg::SetFile(kcl, mClsnMat, 0x1000, mAngleY, data_ov002_0210d8d4);
    mScale = 0x1000;
    return 1;
}

// @symbol _ZN15daObjIceBlock_c8BehaviorEv
s32 daObjIceBlock_c::Behavior() {
    IsClsnInRangeOnScreen(0, 0);

    if (mMeltTimer != 0) {
        mScale = cstd::fdiv(mMeltTimer << 12, kMeltFrames << 12);

        if (DecIfAbove0_Byte(&mMeltTimer) == 0) {
            dActor_c *star = mContainedActor;
            if (star != 0 && star->actorID == kStarActorID)
                ((daStar_c *)star)->unk_49f = 0;
            MarkForDestruction();
        } else {
            Vector3 pos;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            pos.y += FX_MUL(mScale, kShardHeight);

            mParticleHandle1 =
                Particle::System::New(mParticleHandle1, 0x77, pos.x, pos.y, pos.z, 0, 0);
            mParticleHandle2 =
                Particle::System::New(mParticleHandle2, 0x78, pos.x, pos.y, pos.z, 0, 0);

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
        if (mCollider.hitFlags & 0x40000) { /* fire: start melting */
            mMeltTimer = kMeltFrames;
            func_02012694(0x7a, &mCamSpacePos);
            ReleaseStar();
        }
        if (mCollider.hitFlags & 0x4000) /* explosion: shatter */
            Kill();
    }

    mCollider.Clear();
    mCollider.Update();
    return 1;
}

// @symbol _ZN15daObjIceBlock_c6RenderEv
s32 daObjIceBlock_c::Render() {
    Vector3 scale;
    scale.x = mScale;
    scale.y = mScale;
    scale.z = mScale;
    mModel.Render(&scale);
    return 1;
}

// @symbol _ZN15daObjIceBlock_c16CleanupResourcesEv
s32 daObjIceBlock_c::CleanupResources() {
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
void daObjIceBlock_c::Kill() {
    Vector3 pos;
    Vector3 dustPos;
    Sound::PlayBank3(0x41, mCamSpacePos);
    int x = mPosX;
    int y = mPosY + kShardHeight;
    int z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    Particle::System::NewSimple(0x74, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(0x75, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(0x76, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    ReleaseStar();
    MarkForDestruction();
}

// @symbol _ZN15daObjIceBlock_c15OnHitByMegaCharER6Player
void daObjIceBlock_c::OnHitByMegaChar(Player &player) {
    player.IncMegaKillCount();
    Kill();
}

// @symbol _ZN15daObjIceBlock_c11ReleaseStarEv
/* Let the frozen star go: destroy it and respawn it free-standing at the
 * same place and angle, with its star base beside it.
 *
 * The position is read through a Vector3 pointer (the ROM forms star + 0x5c
 * once and loads off it), and the angle is one Vector3_16 struct copy -- three
 * ldrh/strh pairs, where member-by-member s16 reads come out as ldrsh. */
void daObjIceBlock_c::ReleaseStar() {
    dActor_c *star = mContainedActor;
    if (star == 0)
        return;
    if (star->actorID != kStarActorID)
        return;

    Vector3 pos;
    Vector3 *starPos = &star->mPos;
    u32 param1 = star->param1;
    pos.x = starPos->x;
    pos.y = starPos->y;
    pos.z = starPos->z;

    Vector3_16 rot = mContainedActor->mAngle;

    mContainedActor->MarkForDestruction();

    u32 spawnParam = (u8)(param1 & 0xf) | 0x20;
    dActor_c::Spawn(kStarBaseActorID, spawnParam, pos, &rot, mAreaId, -1);
    mContainedActor = dActor_c::Spawn(kStarActorID, spawnParam, pos, &rot, mAreaId, -1);
    mFlags |= 0x4000000;
}

// @symbol _ZN15daObjIceBlock_cD1Ev
// @symbol _ZN15daObjIceBlock_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */

/* Source order is construction order: model file 1114, collision file 1115.
 * __sinit_d_a_obj_ice_block.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
// @symbol __sinit_d_a_obj_ice_block.cpp
IceBlockModelFilePtr data_ov081_02128fd8(ASSET_ICE_BLOCK_LL_BMD);
IceBlockCollisionFilePtr data_ov081_02128fd0(ASSET_ICE_BLOCK_LL_KCL);
