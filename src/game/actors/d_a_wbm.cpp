//cpp
/* Production translation unit for ov098/daWbm_c, hand-curated.
 * 14 functions, .text 0x0213b47c..0x0213bf60.
 *
 * The water bomb. Variant 0 is an invisible dropper that waits for a player
 * to pass beneath it and drops a variant-1 bomb ahead of them. A variant-1
 * bomb falls, squashes and hops toward the closest player, and bursts into
 * five variant-2 fragments on its third landing or when it hits a player.
 * Behavior runs one of three states out of a pointer-to-member table that
 * the module's static initializer fills:
 *
 *   0 StateDrop    dropper: wait for a player below, drop a bomb
 *   1 StateFall    falling
 *   2 StateBounce  squash, stretch and hop after a landing
 *
 * NAME: daWbm_c is the cartridge's RTTI spelling. _ZTS at ov098 0x0213c734
 * is the string "7daWbm_c", and the _ZTI at 0x0213c740 names the vtable at
 * 0x0213c770 as this class's. The tree called the class WaterBomb until then.
 *
 * Land, Burst, StateDrop, StateFall and StateBounce are coined method
 * names; the ROM keeps no names for them. The state table reaches the three
 * state bodies only by address, through the .data words the static
 * initializer copies into it.
 * The destructor is inline and empty in the class header, so Behavior is the
 * key function and this TU emits _ZTV7daWbm_c, _ZTI7daWbm_c and _ZTS7daWbm_c
 * with the inherited bases' RTTI records. D1 and D0 are emitted from the
 * header, so there is no destructor text here to mark. The factory's
 * `new daWbm_c` synthesizes the constructor the ROM inlines into it.
 *
 * Function order is deliberately the reverse of the ROM's. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so
 * the highest-address ROM function, the factory, is written FIRST here. Do
 * not reorder.
 *
 * leftovers:
 * - Player::Hurt and Particle::System::NewSimple stay mangled. Each takes
 *   Fix12<int> by value (wall 6az).
 * - The hit check and the placement helper are members but keep their
 *   func_ov098_* address names.
 * - The fragment angle table (data_ov098_0213bf90) is an unnamed ov098 row
 *   this TU does not own; func_0201267c, the sound call, is an unnamed arm9
 *   function.
 * - Sound ids, particle ids and timer lengths stay numeric.
 */

/* common.h first, so its flat Matrix4x3 is the spelling this TU sees: with
   math/Matrix.h's {Matrix3x3, Vector3} form, the matrix copies in
   func_ov098_0213b584 are memberwise, 0x34 bytes longer. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daWbm_c_AssetId {
    ASSET_WATER_BOMB_BMD = 0x042c
};

#include "common.h"
#include "daWbm_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "math/Approach.h"

typedef void (daWbm_c::*State)();

/* Actor ids are from symbols/actor_debug_names.tsv. */
enum {
    ACTOR_PLAYER = 191,
    ACTOR_BOMBHEI = 206,
    ACTOR_WATERBOMB = 208
};

namespace cstd {
s32 fdiv(s32 numerator, s32 denominator);
}

extern "C" {
/* dExtSimpleModel_c files: the fragment's, which the other variants load too. */
extern SharedFilePtr data_ov002_0210da38;
/* The five fragments' launch angles. */
extern u16 data_ov098_0213bf90[];
/* The sine and cosine table. */
extern s16 data_02082214[];
/* The scratch matrix the matrix helpers build in. */
extern Matrix4x3 data_020a0e68;

void Vec3_Asr(Vector3 *dst, const Vector3 *src, int shift);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, s16 angY);
s16 Vec3_HorzAngle(const Vector3 *from, const Vector3 *to);
s32 Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
u16 DecIfAbove0_Short(u16 *counter);

/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(dBgCh_Actr *self, dActor_c *actor, s32 radius, s32 height, Vector3_16 *a,
                         Vector3_16 *b);
namespace Particle {
struct System {
    static void *NewSimple(u32 id, s32 x, s32 y, s32 z);
};
}
}

/* The bomb's file-scope SharedFilePtr object, spelled through the same
   wrapper idiom the landed folds use: the handle's code is the veneer pair
   the cartridge already carries, so the wrapper gets no bodies of its own.
   The constructor aliases func_02017acc and the destructor func_02017ab4 in
   this unit's manifest. */
struct WbmModelFilePtr : SharedFilePtr {
    WbmModelFilePtr(unsigned int id);
    ~WbmModelFilePtr();
};

/* ------------------------------------------------------------------------
 * The model handle and the state table's three member pointers, in
 * static-initializer order: the initializer constructs the handle, registers
 * its destructor, then copies the pointers from .data. The cannon preloads
 * the same bomb model for the bombs it fires.
 * ------------------------------------------------------------------------ */
// @symbol __sinit_d_a_wbm.cpp
WbmModelFilePtr data_ov098_0213c91c(ASSET_WATER_BOMB_BMD);

/* The member pointers the initializer copies into the table
   (0x0213c71c..0x0213c734 in .data, an unclaimed run). Each record is one
   eight-byte {fn, adj} pair. */
extern State data_ov098_0213c724;
extern State data_ov098_0213c72c;
extern State data_ov098_0213c71c;

State data_ov098_0213c930[3] = {
    data_ov098_0213c724,
    data_ov098_0213c72c,
    data_ov098_0213c71c,
};

// @symbol daWbm_c_classInit
/* Reconstructed source-style name: SM64DS proves daWbm_c through RTTI,
 * allocation size, vtable identity and the WATERBOMB registry profile; later
 * EAD lineage supplies classInit. Historical alias: WaterBomb_Spawn. */
extern "C" int *daWbm_c_classInit(void)
{
    return (int *)new daWbm_c;
}

// @symbol _ZN7daWbm_c13InitResourcesEv
int daWbm_c::InitResources()
{
    BMD_File *bmd;

    mVariant = param1 & 0xf;

    if (mVariant == 2)
    {
        bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_0210da38);
        if (mModel.SetFile(bmd, 1, 0x16) == 0)
            return 0;
    }
    else
    {
        dExtSimpleModel_c::LoadFile(data_ov002_0210da38);
        bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov098_0213c91c);
        if (mModel.SetFile(bmd, 1, 0x16) == 0)
            return 0;
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    if (mVariant == 0)
        mState = 0;
    else
        mState = 1;

    if (mVariant != 0)
    {
        if (mVariant == 2)
        {
            mScaleX = 0x800;
            mScaleY = 0x800;
            mScaleZ = 0x800;
            mdCcAc_c.Init(this, 0x14000, 0x28000, 0x200004, 0);
            mWithMeshClsn.Init(this, 0x1e000, 0x1e000, 0, 0);
        }
        else
        {
            mScaleX = 0x1000;
            mScaleY = 0x1000;
            mScaleZ = 0x1000;
            mdCcAc_c.Init(this, 0x28000, 0x50000, 0x204004, 0);
            mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);
        }
    }

    mStateTimer = 0;
    mBounceCount = 0;
    unk_3b4 = 0;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;

    if ((unsigned int)(mVariant - 1) <= 1)
    {
        mVertAccel = -0x3000;
        mTerminalVelocity = -0x50000;
    }

    return 1;
}

// @symbol _ZN7daWbm_c8BehaviorEv
int daWbm_c::Behavior()
{
    if (func_ov098_0213b6e0()) {
        func_ov098_0213b584();
        mdCcAc_c.Clear();
        return 1;
    }

    (this->*data_ov098_0213c930[mState])();

    if (mVariant != 0) {
        UpdatePos(&mdCcAc_c);
        UpdateWMClsn(mWithMeshClsn, 0);
        if (mWithMeshClsn.IsOnWall()) {
            if (mVariant == 1)
                Burst();
            else
                MarkForDestruction();
            return 0;
        }
        func_ov098_0213b584();
        mdCcAc_c.Clear();
        mdCcAc_c.Update();
    } else {
        DecIfAbove0_Short((u16 *)&mStateTimer);
    }
    return 1;
}

// @symbol _ZN7daWbm_c6RenderEv
int daWbm_c::Render()
{
    if (mVariant != 0) {
        mModel.Render(&mScale);
    }
    return 1;
}

// @symbol _ZN7daWbm_c16CleanupResourcesEv
int daWbm_c::CleanupResources()
{
    if (mVariant == 2) {
        data_ov002_0210da38.Release();
    } else {
        data_ov002_0210da38.Release();
        data_ov098_0213c91c.Release();
    }
    return 1;
}

// @symbol _ZN7daWbm_c9StateFallEv
/* State 1, falling. A bomb that lands squashes and hops; a fragment that
 * lands is gone. A variant-3 bomb ignores the ground and is destroyed once
 * it rises to 6144 units; nothing in this TU spawns one. */
void daWbm_c::StateFall()
{
    if (mVariant != 3) {
        if (mWithMeshClsn.IsOnGround() == 0)
            return;
        if (mVariant == 1) {
            mState = 2;
            Land();
            StateBounce();
            return;
        }
        MarkForDestruction();
        return;
    }
    if (mPosY < 0x1800000)
        return;
    MarkForDestruction();
}

// @symbol _ZN7daWbm_c9StateDropEv
/* State 0, the dropper. Once the cooldown has run out, and the closest player
 * is within 1500 units horizontally and not above the dropper, drop a bomb
 * 1152 units above the player, led ahead of them by their speed. The arithmetic is
 * kept one step per statement: folded, the function is 8 bytes shorter. */
void daWbm_c::StateDrop()
{
    Vector3 spawnPos;
    Player *player;
    s32 angle;
    s32 speed;
    u32 uangle;
    int lead;
    int angleIdx;
    s16 sine, cosine;
    int x, y, z;
    int playerY, bombY;

    if ((u16)mStateTimer != 0)
        return;
    player = ClosestPlayer();
    if (player == 0)
        return;
    {
        const Vector3 &pos = player->mPos;
        spawnPos.x = pos.x;
        spawnPos.y = pos.y;
        spawnPos.z = pos.z;
    }
    if (Vec3_HorzDist(&spawnPos, &mPos) > 0x5dc000)
        return;
    playerY = spawnPos.y;
    bombY = mPosY;
    if (playerY > bombY)
        return;

    angle = player->mPrevAngleY;
    speed = player->mHorzSpeed;
    y = playerY + 0x480000;
    uangle = (u32)(angle << 16) >> 16;
    angleIdx = (int)uangle >> 4;
    lead = speed >> 12;
    lead = lead * 0x1c;
    cosine = data_02082214[angleIdx * 2];
    sine = data_02082214[angleIdx * 2 + 1];
    lead = lead + 0x64;
    x = lead * cosine + spawnPos.x;
    spawnPos.y = y;
    spawnPos.x = x;
    z = lead * sine + spawnPos.z;
    spawnPos.z = z;
    dActor_c::Spawn(ACTOR_WATERBOMB, 1, spawnPos, 0, mAreaId, -1)->mHorzSpeed = 0;
    mStateTimer = 0x96;
    func_0201267c(0xd8, &mCamSpacePos);
}

// @symbol _ZN7daWbm_c11StateBounceEv
/* State 2, after a landing. Phase 0 squashes, and on the third landing the
 * bomb bursts at the bottom of the squash. Phase 1 stretches back up and hops
 * toward the closest player. Phase 2 is the hop; on the way down the bomb is
 * falling again. */
void daWbm_c::StateBounce()
{
    int state = mBouncePhase;
    if (state == 0) {
        if (mBounceCount >= 3) {
            if (ApproachLinear(mSquashScale, 0x100, mSquashSpeed) != 0) {
                Burst();
                return;
            }
        } else {
            if (ApproachLinear(mSquashScale, 0x300, mSquashSpeed) != 0)
                mBouncePhase++;
        }
        mSquashSpeed = cstd::fdiv(FX_MUL(mSquashSpeed, 0xb00), 0x1000);
        mScaleY = mSquashScale;
        mScaleZ = 0x2000 - mScaleY;
        mScaleX = mScaleZ;
    } else if (state == 1) {
        int approach = ApproachLinear(mSquashScale, 0x1000, mSquashSpeed);
        if (approach != 0) {
            Player *player;
            mBouncePhase++;
            mVertSpeed = (mBounceCount << 13) + 0x30000;
            mHorzSpeed = 0x6000;
            player = ClosestPlayer();
            if (player != 0) {
                mPrevAngleY = Vec3_HorzAngle(&mPos, &player->mPos);
            } else {
                Burst();
                return;
            }
        }
        mSquashSpeed = cstd::fdiv(FX_MUL(mSquashSpeed, 0x1400), 0x1000);
        mScaleY = mSquashScale;
        mScaleZ = 0x2000 - mScaleY;
        mScaleX = mScaleZ;
    } else {
        if (mVertSpeed >= 0) {
            mScaleX = 0xf00;
            mScaleY = 0x1200;
            mScaleZ = 0xf00;
        } else {
            mState = 1;
            mScaleX = 0x1000;
            mScaleY = 0x1000;
            mScaleZ = 0x1000;
        }
    }
}

// @symbol _ZN7daWbm_c19func_ov098_0213b6e0Ev
/* The hit check. Touching a player hurts them, and a bomb
 * bursts with a splash; touching a BOMBHEI (actor 206) bursts a bomb too. Returns 1
 * when the bomb burst. */
int daWbm_c::func_ov098_0213b6e0()
{
    Vector3 pos;
    u32 otherID = mdCcAc_c.otherOwner;
    if (otherID != 0) {
        Player *other = static_cast<Player *>(dActor_c::FindWithID(otherID));
        if (other != 0) {
            u16 actorID = other->actorID;
            /* Tested as a flag first: tested inline, or as a switch, the
               function is shorter than the ROM's. */
            int cmp = actorID == ACTOR_PLAYER;
            if (cmp) {
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                other->Hurt(pos, 1, 0xc000, 1, 0, 1);
                if (mVariant == 1) {
                    Particle::System::NewSimple(0x1c, mPosX, mPosY, mPosZ);
                    Particle::System::NewSimple(0x1d, mPosX, mPosY, mPosZ);
                    Burst();
                    return 1;
                }
            } else {
                cmp = actorID == ACTOR_BOMBHEI;
                if (cmp) {
                    if (mVariant == 1) {
                        Burst();
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

// @symbol _ZN7daWbm_c5BurstEv
/* Throw five fragments out at the table's angles, play the splash and go. */
void daWbm_c::Burst()
{
    int i;
    for (i = 0; i < 5; i++) {
        dActor_c *frag =
            dActor_c::Spawn(ACTOR_WATERBOMB, 2, mPos, (const Vector3_16 *)0, mAreaId, -1);
        frag->mSpeedX = 0;
        frag->mVertSpeed = 0x14000;
        frag->mSpeedZ = 0;
        frag->mHorzSpeed = 0x14000;
        frag->mPrevAngleX = 0;
        frag->mPrevAngleY = data_ov098_0213bf90[i];
        frag->mPrevAngleZ = 0;
    }
    func_0201267c(0xda, &mCamSpacePos);
    MarkForDestruction();
}

// @symbol _ZN7daWbm_c19func_ov098_0213b584Ev
/* Place the model and the drop shadow at the bomb's position. */
void daWbm_c::func_ov098_0213b584()
{
    Vector3 shadowPos;
    Vec3_Asr(&shadowPos, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, shadowPos.x, shadowPos.y, shadowPos.z);
    mShadowMat = data_020a0e68;
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    mModel.mat4x3 = data_020a0e68;
    DropShadowRadHeight(mShadowModel, mShadowMat, mScaleX * 0xa0, 0x3e8000, 6);
}

// @symbol _ZN7daWbm_c4LandEv
/* Stop, start the squash from the current height and count the bounce. */
void daWbm_c::Land()
{
    mSquashSpeed = 0x600;
    mSquashScale = mScaleY;
    mBounceCount++;
    unk_3b4 = 0;
    mVertSpeed = 0;
    mHorzSpeed = 0;
    mBouncePhase = 0;
    func_0201267c(0xd9, &mCamSpacePos);
}
