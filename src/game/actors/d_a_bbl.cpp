//cpp
/* daBbl_c -- Lava Bubble (Podoboo), BUBBLE, ov064 0x021185c0..0x02118b50.
 * The spawn parameter's low bit picks a fixed flame or the jumping bubble.
 * Both use the same two-entry state table. Source order is the reverse of
 * the ROM. Do not reorder. The inline destructor in include/daBbl_c.h emits
 * D1 then D0 and no D2.
 */
/* leftovers: Particle::System::New, dActor_c::IsTooFarAwayFromPlayer,
 * dCcAc_c::Init and dBgCh_Actr::Init stay C-ABI externs. This file calls
 * them, and each definition passes Fix12<int> by value. The five state
 * hooks keep address names; the ROM records no English name. The State
 * tables at 0x0211c7b8 and 0x0211c7c8 are filled from this file's static
 * initializer. func_ov064_02118760 stores the spawn words y, z, x through
 * a volatile array; a plain Vector3 lets the compiler reorder those stores.
 */

#include "daBbl_c.h"
#include "Player.h"

/* Ramp `value` toward `target` by at most `step`. The mangled name this
 * declaration produces is the compiler's own answer, not a hand-mangle. */
void ApproachLinear(int &value, int target, int step);

extern "C" {
/* Counts a halfword timer down and returns what it was; a short* helper, so
 * the slot it is handed is a HALFWORD. */
u16 DecIfAbove0_Short(u16 *timer);

/* The six-argument particle spawner: it takes the handle it last returned,
 * rolls it forward and hands back the new one. Kept at its C ABI spelling
 * because its real declaration passes three Fix12<int> by value; the trailing
 * two parameters are spelt as its definition spells them. */
struct Vec3;
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 handle, u32 effectID, int x, int y, int z, const Vec3 *rot,
    void *callback);

/* True while the player is further than `dist` away. Fix12<int> by value
 * again. */
int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(dActor_c *self, int dist);

/* The actor-collision cylinder's own initialiser; two more Fix12<int> by
 * value, so it stays at its C ABI spelling too. */
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *actor, int radius, int height,
    u32 flags, u32 vulnFlags);

/* The mesh collider's initialiser. include/dBgCh_Actr.h declares it with the
 * scalar Fix12i spelling for readers, but the definition the cartridge links
 * carries Fix12<int> in its own name, so the call site has to spell the ABI
 * seam -- and that definition takes every argument past the receiver as a
 * plain word, which is why the owner is handed over as one. */
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, int actor, int radius, int height, int rotA, int rotB);

/* Eight-byte pointer-to-member records. The cartridge stores the four
 * constants in .data and copies them into the two .bss tables below. */
struct BblPmfPair { int ptr; int adj; };
extern BblPmfPair data_ov064_0211bea0;
extern BblPmfPair data_ov064_0211be90;
extern BblPmfPair data_ov064_0211bea8;
extern BblPmfPair data_ov064_0211be98;
extern BblPmfPair data_ov064_0211c7b8[2];
extern BblPmfPair data_ov064_0211c7c8[2];
}

// @symbol daBbl_c_classInit
/* The BUBBLE profile's factory. `new` alone reproduces the body: the 0x31c
 * leaf operator new, the base C2, the vptr store and the two member
 * constructors inline -- a declared constructor would emit a `bl` the
 * factory does not have. */
extern "C" daBbl_c *daBbl_c_classInit()
{
    return new daBbl_c();
}

// @symbol _ZN7daBbl_c13OnYoshiTryEatEv
/* Yoshi is told 5 -- the reply a thing made of fire gives. */
s32 daBbl_c::OnYoshiTryEat()
{
    return 5;
}

// @symbol _ZN7daBbl_c13InitResourcesEv
/* The spawn parameter's low bit picks the variant, inverted: bit set means
 * the fixed flame, bit clear means the jumping bubble.
 *
 * The flame gets a bare flag-1 cylinder and dActor_c's own 0x1 flag, which
 * pins it in place; the jumper gets a 50.0 x 80.0 cylinder that can be hit
 * (0x200002 / 0x8000) plus gravity and a terminal velocity. Both remember
 * where they started, and both adopt a state table before returning. */
int daBbl_c::InitResources()
{
    mJumps = (param1 & 1) ^ 1;
    if (mJumps == 0) {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0, 0, 1, 0);
    } else {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
            &mdCcAc_c, this, 0x32000, 0x50000, 0x200002, 0x8000);
    }
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    if (mJumps == 0) {
        mFlags |= 1;
    } else {
        mVertAccel = -0x4000;
        mTerminalVelocity = -0x3c000;
    }
    mStateTimer = 0;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, (int)this, 0x32000, 0x32000, 0, 0);
    mWithMeshClsn.SetLimMovFlag();
    if (mJumps != 0) {
        func_ov064_021187ec((State *)data_ov064_0211c7c8);
    } else {
        func_ov064_021187ec((State *)data_ov064_0211c7b8);
    }
    return 1;
}

// @symbol _ZN7daBbl_c8BehaviorEv
/* Two early outs come first, on the two dEnemyBase_c flags that mean the
 * enemy is being held or is already dying: 0x20000 still runs the state hook
 * but skips everything else, 0x40000 skips the frame outright.
 *
 * Then the ordinary frame: destroy the actor once the player is more than
 * 1500.0 away -- but only the jumping variant, which a flame fountain
 * respawns; the fixed flame is part of the level. Count the state timer
 * down, burn whatever touched the cylinder if it is the player, run the
 * state hook, integrate position, and -- only for the variant that has
 * gravity -- collide with the mesh. */
int daBbl_c::Behavior()
{
    int flags = mFlags;
    int held = (flags & 0x20000) != 0;

    if (held) {
        if (mState->mExecute != 0)
            (this->*(mState->mExecute))();
        return 1;
    }

    {
        int dying = (flags & 0x40000) != 0;
        if (dying)
            return 1;
    }

    if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(this, 0x5dc000)) {
        if (mJumps != 0)
            MarkForDestruction();
        return 1;
    }

    DecIfAbove0_Short((u16 *)&mStateTimer);

    {
        u32 otherID = mdCcAc_c.otherOwner;
        if (otherID != 0) {
            if ((mdCcAc_c.hitFlags & 0x8000) == 0) {
                dActor_c *other = dActor_c::FindWithID(otherID);
                if (other != 0) {
                    int isPlayer = (other->actorID == 0xbf);
                    if (isPlayer)
                        ((Player *)other)->Burn();
                }
            } else {
                mdCcAc_c.flags |= 1;
            }
        }
    }

    if (mState->mExecute != 0)
        (this->*(mState->mExecute))();

    UpdatePos(&mdCcAc_c);

    if (mVertAccel != 0)
        UpdateWMClsn(mWithMeshClsn, 0);

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN7daBbl_c6RenderEv
/* A lava bubble has no model of its own; its two particle systems draw it,
 * and the render slot only reports success. */
int daBbl_c::Render()
{
    return 1;
}

// @symbol _ZN7daBbl_c16OnPendingDestroyEv
/* Empty on purpose -- the override suppresses what the base does on pending
 * destroy. */
void daBbl_c::OnPendingDestroy()
{
}

// @symbol _ZN7daBbl_c16CleanupResourcesEv
/* `return 1` with no release calls: daBbl_c loads no file at all, so it has
 * nothing to give back. */
int daBbl_c::CleanupResources()
{
    return 1;
}

// @symbol _ZN7daBbl_c19func_ov064_021187ecEPNS_5StateE
/* The state transition: adopt a table and run its enter hook once. Behavior
 * runs the execute hook of whatever table is current. */
int daBbl_c::func_ov064_021187ec(State *state)
{
    mState = state;

    State *entered = mState;
    if (entered->mEnter == 0)
        return 1;
    return (this->*(entered->mEnter))();
}

// @symbol _ZN7daBbl_c19func_ov064_021187d0Ev
/* Enter hook of the fountain's waiting state: stop dead and wait 180 frames. */
int daBbl_c::func_ov064_021187d0()
{
    mHorzSpeed = 0;
    mStateTimer = 180;
    return 1;
}

// @symbol _ZN7daBbl_c19func_ov064_02118760Ev
/* Execute hook of the waiting state: once the 180 frames are up, spawn actor
 * 0xd6 -- the bubble that actually jumps -- 120.0 above this one, and start
 * the wait over. This is what makes the fixed flame a flame fountain. */
int daBbl_c::func_ov064_02118760()
{
    int spawnY;
    int spawnZ;

    if ((u16)mStateTimer == 0) {
        volatile int spawnPos[3];

        mStateTimer = 0xb4;
        int y = mPosY;
        int z = mPosZ;
        spawnY = 0x78000;
        spawnY = y + spawnY;
        spawnZ = z;
        int x = mPosX;
        spawnPos[1] = spawnY;
        spawnPos[2] = spawnZ;
        spawnPos[0] = x;
        dActor_c::Spawn(0xd6, 0, *(const Vector3 *)spawnPos, &mPrevAngle, mAreaId, -1);
    }
    return 1;
}

// @symbol _ZN7daBbl_c19func_ov064_0211873cEv
/* Enter hook of the jumping state: no horizontal drift, 20.0 straight up,
 * and a 135-frame fuse. */
int daBbl_c::func_ov064_0211873c()
{
    mHorzSpeed = 0;
    mVertSpeed = 0x14000;
    mStateTimer = 135;
    return 1;
}

// @symbol _ZN7daBbl_c19func_ov064_02118644Ev
/* Execute hook of the jumping state. Every landing bounces: the first gives
 * the bubble a little forward drift and a fixed hop, and each one after that
 * ramps the drift toward 8.0 and inverts seven tenths of the vertical speed,
 * so the hops decay. The fire and smoke particles are rolled forward every
 * frame 55.0 above the bubble, and the fuse the enter hook set is what
 * finally destroys it. */
int daBbl_c::func_ov064_02118644()
{
    if (mWithMeshClsn.IsOnGround() != 0) {
        if (mHorzSpeed == 0) {
            mHorzSpeed = 0x16000;
            mVertSpeed = 0x32000;
        } else {
            ApproachLinear(mHorzSpeed, 0x8000, 0x3000);
            mVertSpeed = mVertSpeed * -7 / 10;
        }
    }
    mFireParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mFireParticle, 0x4d, mPosX, mPosY + 0x37000, mPosZ, 0, 0);
    mSmokeParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mSmokeParticle, 0x4e, mPosX, mPosY + 0x37000, mPosZ, 0, 0);
    if ((u16)mStateTimer == 0)
        MarkForDestruction();
    return 1;
}

/* Declaration order is the retail __sinit order: the fountain table, then
 * the jumping table. Each entry is one pointer-to-member pair. */
extern "C" {
BblPmfPair data_ov064_0211c7b8[2] = { data_ov064_0211bea0, data_ov064_0211be90 };
BblPmfPair data_ov064_0211c7c8[2] = { data_ov064_0211bea8, data_ov064_0211be98 };
}
