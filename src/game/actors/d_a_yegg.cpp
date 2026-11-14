//cpp
/* daYegg_c -- the Yoshi egg (registry profile YOSHI_EGG).
 *
 * The egg a Yoshi carries and throws. The state table at 0x02110a5c holds
 * two member-function pointers per state, enter and execute (see daYegg_State
 * in daYegg_c.h), and the state number is mState: func_ov002_020ed63c installs
 * a state and runs its enter, RunState runs the current execute
 * from Behavior. An egg in state 0 trails the player (mPlayer;
 * ExecuteFollowPlayer, EnterFollowPlayer); daPly_c::St_YoshiPower_Init takes
 * one from the daPly_c's held-object queue, sets its 0x100 flag bit and enters
 * state 1, where it homes on the nearest actor not yet targeted
 * (FindTarget), and on contact or
 * after five payouts it bursts (func_ov002_020edca4), paying out coins, a
 * blue coin or the tracked star as param1 asks (HasStarPayout /
 * 020ec628 / 020ec640 read the bits of mParamHigh, which is param1 >> 4).
 * param1 bits 0-1 are the starting state (and the egg's size), bits 4-5 the
 * variant func_ov002_020ec654 tests, bits 6-9 the coin count, bit 10 the blue
 * coin and bit 11 the star.
 *
 * This file is the whole linker unit 0x020ec56c..0x020ee42c, 33 functions:
 * D1 and D0 (src/game/actors/d_a_warpkun.cpp ends exactly at 0x020ec56c
 * below them), the twenty-six helpers HasStarPayout through
 * HandleCylinderHit, CleanupResources, Render, Behavior, InitResources,
 * and last the registry factory daYegg_c_classInit (0x020ee3d8);
 * src/runtime/actor/d_bg_actor.cpp starts exactly at 0x020ee42c above it. The
 * out-of-line destructor is the key function, so this TU also emits the
 * vtable and the RTTI.
 *
 * It replaces the one-function sources for _ZN8daYegg_cD1Ev,
 * _ZN8daYegg_cD0Ev, HasStarPayout .. HandleCylinderHit,
 * _ZN8daYegg_c16CleanupResourcesEv, _ZN8daYegg_c6RenderEv,
 * _ZN8daYegg_c8BehaviorEv, _ZN8daYegg_c13InitResourcesEv and
 * daYegg_c_classInit. Each member keeps the provenance notes its source
 * carried.
 *
 * `#pragma defer_codegen off` keeps this file in ROM order.
 *
 * The twenty-four self-first helpers are real daYegg_c members; the two
 * called from daPly_c code (func_ov002_020ed63c by St_YoshiPower_Init,
 * func_ov002_020edca4 by d_a_ply_methods.cpp) stay free. All keep their address-based
 * names; the comments say what each does, not what the original called it.
 *
 * Actor targeting uses dActor_c's existing virtual interfaces: slot 30 returns
 * the aim position, and slot 20 supplies the integer query used by the variant.
 * The shadow ground probe is a real dBgCh_Gnd, constructed after the opacity
 * guard and position setup, with its result read through clsnY. These interface
 * names are reconstructed; this slice does not recover original method names.
 *
 * deslop leftovers:
 * - the callees with Fix12<int> parameters (dExtAnmModel_c::SetAnim,
 *   dBgCh_Actr::Init, Particle::System::New) stay spelled as mangled
 *   extern-C free functions. A real method call homes
 *   a class-typed by-value argument and size-DIFFs the caller
 *   (notes/mwccarm-codegen.md 6az).
 * - the DOSUN byte at +0x3a2 in HandleWallHit remains a raw offset.
 *   Spawned coins use dActor_c's existing previous-angle and horizontal-speed fields.
 * - mSpawnPos is named in daYegg_c.h but its purpose is not known
 *   (written once, never read here); the 4-byte gap at 0x3f8 is unnamed.
 * - this list is representative, not exhaustive: the flag and mask
 *   literals (mFlags 0x40000/0x10000000/0x8, the hit flags 0x8000/0x26fe0,
 *   the cylinder words 0x202000/0x200002/0xa08000) are also left numeric, and
 *   the extern-C Fix12 callees include SpawnCoins, daPly_c::Hurt,
 *   Particle::NewSimple and RunningSlidingDustAt as well.
 * - the particle, sound, shadow and animation-file arguments are
 *   numeric (0x3f..0x42, 0x2c, 0x93, 0x103, the two shared animation files)
 *   and the egg's flag bits 0x100/0x400/0x2000 are named by use only. The IDs'
 *   names are not recovered. 0x100 is set by daPly_c::St_YoshiPower_Init (on the
 *   egg it takes from the held-object queue); who sets 0x400 and 0x2000 is not
 *   recovered. The "idle" and "moving" animation names say when the code
 *   picks each file, not recovered names.
 * - PlayCharVoice keeps its void-returning local decl (Sound.h declares u32): a return-width wall, do not convert.
 */

#pragma defer_codegen off

#include "daYegg_c.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "dBgCh_Gnd.h"
#include "dActor_c.h"
#include "dBgCh_Actr.h"
#include "dCc_c.h"
#include "Sound.h"
#include "daStar_c.h"
#include "daBtn_c.h"
#include "math/Approach.h"
#include "private/mtx43.h"
#include "LevelRng.h"
#include "nitro/fx/fx_cp.h"

struct dBgPi;

/* Actor registry IDs this file tests or spawns (symbols/actor_debug_names.tsv). */
enum {
    ACTOR_DOSUN       = 0xa1,
    ACTOR_BATAN       = 0xa4,
    ACTOR_BATANKING   = 0xa5,
    ACTOR_SILVER_STAR = 0xb3,
    ACTOR_BOMBKING    = 0xbd,
    ACTOR_HOLHEI      = 0xbe,
    ACTOR_PLAYER      = 0xbf,
    ACTOR_KURIKING    = 0xc6,
    ACTOR_KURIBO_L    = 0xca,
    ACTOR_WANWAN      = 0xdb,
    ACTOR_BLUE_COIN   = 0x122,
    ACTOR_WANWAN2     = 0x151
};

/* The state table at 0x02110a5c: two member-function pointers per state,
   enter and execute, indexed by mState. __sinit_ov002_02107118 fills it. */
typedef void (daYegg_c::*PMF)();
struct Entry { PMF pmf[2]; };
extern Entry data_ov002_02110a5c[];

namespace Sound {
/* local extern: including Sound.h renumbers @NNN. */
int PlayLong(u32, u32, u32, const Vector3 &, s16);
}

extern "C" {
/* Defined below. */

/* Data. 0x0210e6b0 and 0x0210eb78 are the two shared animation file
   handles; d_a_ply_methods.cpp's static initializer constructs both. */
extern SharedFilePtr data_ov002_0210e6b0;
extern SharedFilePtr data_ov002_0210eb78;
extern s16 data_ov002_021000a8[];
/* local extern: math/MtxStack.h's spellings of the Matrix4x3_ calls below
   clash with these. The copies go through the flat Mtx43: assigning this
   TU's nested Matrix4x3 grows the object by 0x30 bytes. */
extern Matrix4x3 data_020a0e68;

extern int func_02035638(u8 *p);
extern char *data_ov002_021000a0[];
extern void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
/* Was spelled `func_02123804`, a name no symbols.txt defines. ov002's relocs
   record the call at 0x020ec718 as `overlays(77,78,79,80)`: four same-base
   overlays each put a function at 0x02123804. The call is gated on actor
   types 0xa4 and 0xa5, BATAN and BATANKING, both daBtn_c in ov079, and
   ov079's function there is daBtn_c's hit reaction, which takes the body and
   the actor that hit it -- the call's (actor, egg). */
extern int RandomIntInternal(int *seed);
/* Numbering pins: func_ov002_020d5f98/020d6048 are now daPly_c members (see
   daPly_c.h) and no call in this TU uses these free spellings anymore. The
   declarations stay, spelled exactly as they were, because each named
   parameter advances mwccarm's @NNN counter: removing or un-naming them
   renumbers this TU's .data temps and rotates __sinit_d_a_yegg.cpp's pool
   destinations (measured). */
extern int func_ov002_020d5f98(void *c, unsigned char *a, unsigned char *b);
extern int func_ov002_020d6048(void *c);
extern s32 d_a_yeggNewHold(u32 a, u32 b, s32 x, s32 y, s32 z, Vector3_16 *rot, void *cb);
extern void d_a_yeggRunningSlidingDustAtHold(s32 x, s32 y, s32 z);
extern void d_a_yeggPlayLongHold(u32 a, u32 b, u32 c, void *v, u32 d);
extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern void MTX_MultVec43(Vector3 *in, Matrix4x3 *m, Vector3 *out);
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, void *actor, int radius, int height, void *a, void *b);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern int Math_Function_0203b14c(void *ptr, int target, int rate, int limit, int step);
extern int ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *m, int x, int y, int z);
extern int func_ov002_020cf700(void *g);
}
void func_ov002_020ed63c(daYegg_c *c, int i);
void func_ov002_020edca4(daYegg_c *self);

/* One native destructor definition emits both ROM variants. D1 is one vtable
 * store and five destructor calls, every one a consequence of
 * `struct daYegg_c : dEnemyBase_c` and the four members that declaration
 * types, destroyed in reverse declaration order, then dEnemyBase_c; the
 * class adds no member with a destructor of its own -- a daPly_c pointer and
 * scalars. D0 is the same destruction followed by the inherited actor-heap
 * operator delete. Being the first out-of-line virtual, it makes this file the
 * key function's home, so the vtable and RTTI are emitted here too. */
daYegg_c::~daYegg_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- HasStarPayout, 0x020ec610, size 0x18 */
/* -------------------------------------------------------------------------- */
/* Reads mParamHigh bit 7 (param1 bit 11): nonzero when the egg pays out the
 * tracked star when it bursts. */
int daYegg_c::HasStarPayout() {
    return ((mParamHigh >> 7) & 1) != 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- HasBlueCoinPayout, 0x020ec628, size 0x18 */
/* -------------------------------------------------------------------------- */
/* Reads mParamHigh bit 6 (param1 bit 10): nonzero when the egg pays out a
 * blue coin. InitResources loads the blue-coin model for it and
 * CleanupResources unloads it. */
int daYegg_c::HasBlueCoinPayout() {
    return ((mParamHigh >> 6) & 1) != 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- GetCoinPayoutCount, 0x020ec640, size 0x14 */
/* -------------------------------------------------------------------------- */
/* Reads mParamHigh bits 2-5 (param1 bits 6-9): the number of coins to pay out,
 * 0 to 15. */
unsigned char daYegg_c::GetCoinPayoutCount() {
    return (mParamHigh >> 2) & 0xf;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov002_020ec654, 0x020ec654, size 0x1c */
/* -------------------------------------------------------------------------- */
/* The egg's variant bit: reads mParamHigh bits 0-1 (param1 bits 4-5) and is
 * nonzero when that value is 0 or 2, zero when it is 1 or 3. It chooses the
 * model, the shadow shape (cylinder when zero, cuboid when nonzero), the
 * flight handler in state 1 and the state-1 enter's gravity, target-search
 * range and ground-collision setup (the cylinder it builds is the same for
 * both). What the two variants are in the game is not recovered. */
int daYegg_c::func_ov002_020ec654() {
    return (((mParamHigh & 3) - 1) & 1) != 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- HandleWallHit, 0x020ec670, size 0xb8 */
/* -------------------------------------------------------------------------- */
/* What the egg did to the wall it hit. `arg` is the egg's dBgCh_Actr as an
 * integer; dBgCh_Actr::GetSphereResult turns it into a dBgPi, whose collision ID names the
 * actor owning the surface hit. Returns without doing anything if there is no
 * such ID or actor. A DOSUN gets its byte at +0x3a2 set to 1; a BATAN or
 * BATANKING is handed the egg through daBtn_c's hit reaction. Any other actor
 * is left alone. */
void daYegg_c::HandleWallHit(int arg) {
    struct dBgPi* cr;
    struct dActor_c* actor;
    u16 type;

    cr = ((dBgCh_Actr *)arg)->GetSphereResult();
    if (cr->GetClsnID() == -1)
        return;

    actor = dActor_c::FindWithID(cr->GetClsnID());
    if (actor == 0) return;

    type = actor->actorID;
    {
        int t = (int)(type == ACTOR_DOSUN);
        if (t != 0) {
            *(u8*)((char*)actor + 0x3a2) = 1;
            return;
        }
    }
    {
        int t = (int)(type == ACTOR_BATAN);
        if (t == 0) {
            t = (int)(type == ACTOR_BATANKING);
            if (t == 0)
                return;
        }
    }
    ((daBtn_c *)actor)->func_ov079_02123804(this);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- PayOut, 0x020ec728, size 0xe4 */
/* -------------------------------------------------------------------------- */
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* Pays out once for the current payout slot (mPayoutIdx). Does nothing if the
 * index is already 5 or the slot is already marked paid. Otherwise it spawns
 * the coin count from GetCoinPayoutCount as coins, and one blue coin if
 * HasBlueCoinPayout says so, both at the egg's position raised by 0x28000
 * (40 units), then marks the slot paid. The 0x2000 handed on is 2.0 in
 * Fix12; what that argument controls is not recovered here. */
void daYegg_c::PayOut() {
    unsigned int idx = mPayoutIdx;
    unsigned int n;
    struct Vector3 vec;
    if (idx >= 5) return;
    if (mPayoutDone[idx] != 0) return;
    n = GetCoinPayoutCount();
    {
        int tx = mPosX;
        int tz = mPosZ;
        int ty = mPosY + 0x28000;
        vec.x = tx;
        vec.y = ty;
        vec.z = tz;
    }
    if (n != 0) {
        struct Vector3 v2;
        v2.x = vec.x;
        v2.y = vec.y;
        v2.z = vec.z;
        SpawnCoins(v2, n, 0x2000, 0);
    }
    if (HasBlueCoinPayout()) {
        struct Vector3 v3;
        v3.x = *(int*)&vec.x;
        v3.y = *(int*)&vec.y;
        v3.z = *(int*)&vec.z;
        SpawnBlueCoins(&v3, 1, 0x2000, 0);
    }
    mPayoutDone[mPayoutIdx] = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- SpawnBlueCoins, 0x020ec80c, size 0x12c */
/* -------------------------------------------------------------------------- */
/* Spawns `count` blue coins (actor BLUE_COIN, param1 2) at *b in the egg's
 * area. When more than one is asked for, the speed `sl` is raised to at least
 * 0x4000 (4 units). Each coin gets a heading of `arg5` plus a random multiple
 * of 0x800 (a 32nd of a turn) that differs from the previous coin's, and a
 * speed of `sl` scaled by a random 100 to 149 percent. The scaling compounds,
 * because the product is stored back into `sl`. The four fields written at
 * +0x92, +0x94, +0x96 and +0x98 of each new actor (three halfwords and a
 * word) are dActor_c's
 * mPrevAngleX/Y/Z and mHorzSpeed; whether the coin reads them as spawn
 * arguments is not shown here. */
void daYegg_c::SpawnBlueCoins(void *b, int count, int sl, short arg5) {
    unsigned int r;
    int zero;
    dActor_c *coin;
    int rv;
    int prev;
    int i;
    prev = 0xff;
    if (count > 1) {
        if (sl < 0x4000)
            sl = 0x4000;
    }
    i = 0;
    if (count <= 0)
        return;
    zero = i;
    do {
        coin = dActor_c::Spawn(ACTOR_BLUE_COIN, 2, *(const Vector3 *)b, 0, mAreaId, -1);
        if (coin != 0) {
            do {
                rv = (int)(((unsigned int)RandomIntInternal(&data_0209e650.value) >> 16) << 27) >>
                     16;
            } while (rv == prev);
            r = RandomIntInternal(&data_0209e650.value);
            r = r >> 16;
            coin->mPrevAngleX = zero;
            sl = sl * ((r % 50) + 100) / 100;
            prev = rv;
            coin->mPrevAngleY = arg5 + rv;
            coin->mPrevAngleZ = zero;
            coin->mHorzSpeed = sl;
        }
        i++;
    } while (i < count);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- ExecuteDrop, 0x020ec938, size 0x40 */
/* -------------------------------------------------------------------------- */
/* State 3 (DROP), execute: moves the egg by its speed and gravity, bursts it
 * if it is on the ground, then runs the ground collision and clears the
 * cylinder's hit record. */
void daYegg_c::ExecuteDrop() {
    UpdatePos(&mdCcAc_c);
    if (mWithMeshClsn.IsOnGround())
        func_ov002_020edca4(this);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    mdCcAc_c.Clear();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- EnterDrop, 0x020ec978, size 0x4c */
/* -------------------------------------------------------------------------- */
/* State 3 (DROP), enter: plays the idle animation (file 0x0210eb78) at normal
 * speed, zeroes the horizontal speed and sets gravity to -0x2000 (-2 units
 * per frame squared). */
void daYegg_c::EnterDrop() {
    mAnmModel.SetAnim((BCA_File *)data_ov002_0210eb78.filePtr, 0, 0x1000, 0);
    mHorzSpeed = 0;
    mVertAccel = -0x2000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- ExecuteWobble, 0x020ec9c4, size 0x110 */
/* -------------------------------------------------------------------------- */
/* State 2 (WOBBLE), execute. While mWobbling is set it steps the X and Z tilt
 * (mAngleX, mAngleZ) toward 0 by 0x400 a frame (a 64th of a turn) and clears
 * mWobbling once the X tilt and the Y angle are both 0. Otherwise it asks
 * func_ov002_020d5f98 about the daPly_c; on a yes, it sets the tilt from
 * the answer (b times two entries of the s16 table at 0x021000a8, indexed by
 * a*2) and sets mWobbling. Then, every frame: destroys the egg if
 * func_ov002_020d6048 says no for the daPly_c, clears and updates the
 * cylinder, moves the egg when it is not on the ground, and runs the ground
 * collision. */
void daYegg_c::ExecuteWobble() {
    if (mWobbling != 0) {
        ApproachLinear2(mAngleX, 0, 0x400);
        ApproachLinear2(mAngleZ, 0, 0x400);
        if (mAngleX == 0) {
            if (mAngleY == 0)
                mWobbling = 0;
        }
    } else {
        unsigned char a, b;
        if (mPlayer->func_ov002_020d5f98(&a, &b)) {
            unsigned char ip;
            a = a * 2;
            ip = a;
            mAngleX = b * data_ov002_021000a8[ip];
            mAngleZ = b * data_ov002_021000a8[ip + 1];
            mWobbling = 1;
        }
    }
    if (!mPlayer->func_ov002_020d6048())
        MarkForDestruction();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    if (!mWithMeshClsn.IsOnGround())
        UpdatePos(&mdCcAc_c);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- EnterWobble, 0x020ecad4, size 0x38 */
/* -------------------------------------------------------------------------- */
/* State 2 (WOBBLE), enter: zeroes the horizontal speed and plays the idle
 * animation (file 0x0210eb78) at normal speed. */
void daYegg_c::EnterWobble() {
    mHorzSpeed = 0;
    mAnmModel.SetAnim((BCA_File *)data_ov002_0210eb78.filePtr, 0, 0x1000, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- SeekStraight, 0x020ecb0c, size 0x20c */
/* -------------------------------------------------------------------------- */
/* State 1 (SEEK), execute, for the egg variant func_ov002_020ec654 calls
 * nonzero (see ecf94). If the actor it is seeking (mTargetId) exists and no
 * longer has profile bit 0x10000000, it pays out and re-enters state 1 so a
 * new target is picked. Otherwise, unless daYegg_FLAG_ATTACHED is set: with
 * daYegg_FLAG_DROPPED it enters state 3; with daYegg_FLAG_FLYING it feeds
 * its saved handles back into Particle::System::New (0x2c) and
 * Sound::PlayLong (3, 0x93), runs UpdateTilt (tilt), returns if
 * HandleCylinderHit handled a contact (nonzero), and bursts if IsOnWall is
 * set or bit 0x10 of the same result byte is. The particle's rotation pair comes from the sine
 * table indexed by mAngleY >> 4 (sin then cos). Every pass that gets this far
 * bursts if IsStalled reports the egg stalled, and otherwise clears
 * and, if flying, updates the cylinder, moves the egg and runs the ground
 * collision. */
void daYegg_c::SeekStraight() {
    dActor_c *o;
    u32 id;
    s32 flag;
    s32 b;
    u32 fb0;
    s32 onwall;
    Vector3_16 rot;

    id = mTargetId;
    o = 0;
    if (id != 0)
        o = dActor_c::FindWithID(id);
    if (o != 0) {
        b = ((o->mFlags & 0x10000000) != 0);
        if (b == 0) {
            PayOut();
            func_ov002_020ed63c(this, daYegg_STATE_SEEK);
            return;
        }
    }
    fb0 = mFlags;
    flag = 0;
    b = ((fb0 & daYegg_FLAG_ATTACHED) != 0);
    if (b == 0) {
        b = ((fb0 & daYegg_FLAG_DROPPED) != 0);
        if (b != 0) {
            func_ov002_020ed63c(this, daYegg_STATE_DROP);
        } else {
            b = ((fb0 & daYegg_FLAG_FLYING) != 0);
            if (b != 0) {
                Vector3 pos(mPos.x, mPos.y + 0x28000, mPos.z);
                rot.x = FX_SinIdx((u16)mAngleY >> 4);
                rot.y = 0;
                rot.z = FX_CosIdx((u16)mAngleY >> 4);
                mParticleHandle = Particle::System::New(mParticleHandle, kPtclYoshiEggTrail, pos.x,
                                                        pos.y, pos.z, (const Vector3_16f *)&rot, 0);
                mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x93, mCamSpacePos, 0);
                flag = 1;
                UpdateTilt();
                if (HandleCylinderHit() != 0)
                    return;
                onwall = mWithMeshClsn.IsOnWall();
                if ((onwall | func_02035638((u8 *)&mWithMeshClsn)) != 0) {
                    func_ov002_020edca4(this);
                    return;
                }
            }
        }
    }
    if (IsStalled() != 0) {
        func_ov002_020edca4(this);
        return;
    }
    mdCcAc_c.Clear();
    if (flag == 1)
        mdCcAc_c.Update();
    UpdatePos(&mdCcAc_c);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- SeekHoming, 0x020ecd18, size 0x27c */
/* -------------------------------------------------------------------------- */
/* State 1 (SEEK), execute, for the other egg variant (see ecf94). `o` is the
 * actor being sought (mTargetId). If it is gone, or has lost profile bit
 * 0x10000000, the egg re-enters state 1 to pick again. Unless
 * daYegg_FLAG_ATTACHED is set: with daYegg_FLAG_DROPPED it enters state 3; with
 * daYegg_FLAG_FLYING it checks the wall hit record first (HandleWallHit,
 * then bursts), then, if it has a target, moves toward the position that
 * actor's vtable slot 30 returns, at mHorzSpeed per step. Having arrived, it
 * bursts when the target is one of the six burst actors (BOMBKING, HOLHEI,
 * KURIKING, KURIBO_L, WANWAN, WANWAN2) and otherwise pays out and re-enters
 * state 1; if not yet there it turns to face the target. Flying then kicks up
 * sliding dust at its position and runs HandleCylinderHit (a nonzero result
 * ends the frame). Every pass bursts if IsStalled reports the egg
 * stalled, otherwise clears and, if flying, updates the cylinder, moves the
 * egg only when it has no target, and runs the ground collision. */
void daYegg_c::SeekHoming() {
    dActor_c *o;
    s32 flag;
    s32 b;
    u32 fb0;

    o = 0;
    if (mTargetId != 0)
        o = dActor_c::FindWithID(mTargetId);
    if (o != 0) {
        b = ((o->mFlags & 0x10000000) != 0);
        if (b == 0) {
            func_ov002_020ed63c(this, daYegg_STATE_SEEK);
            return;
        }
    }
    if (mTargetId != 0 && o == 0) {
        func_ov002_020ed63c(this, daYegg_STATE_SEEK);
        return;
    }
    fb0 = mFlags;
    flag = 0;
    b = ((fb0 & daYegg_FLAG_ATTACHED) != 0);
    if (b == 0) {
        b = ((fb0 & daYegg_FLAG_DROPPED) != 0);
        if (b != 0) {
            func_ov002_020ed63c(this, daYegg_STATE_DROP);
        } else {
            b = ((fb0 & daYegg_FLAG_FLYING) != 0);
            if (b != 0) {
                if (mWithMeshClsn.GetResultFlag1() != 0) {
                    HandleWallHit((int)&mWithMeshClsn);
                    func_ov002_020edca4(this);
                    return;
                }
                if (o != 0) {
                    Vector3 tmp = o->OnAimedAtWithEggReturnVec();
                    if (ApproachLinear(mPos, tmp, mHorzSpeed) != 0) {
                        switch (o->actorID) {
                        case ACTOR_BOMBKING:
                        case ACTOR_HOLHEI:
                        case ACTOR_KURIKING:
                        case ACTOR_KURIBO_L:
                        case ACTOR_WANWAN:
                        case ACTOR_WANWAN2:
                            func_ov002_020edca4(this);
                            return;
                        default:
                            PayOut();
                            func_ov002_020ed63c(this, daYegg_STATE_SEEK);
                            return;
                        }
                    }
                    mAngleY = Vec3_HorzAngle(&mPos, &tmp);
                }
                Particle::RunningSlidingDustAt(mPosX, mPosY, mPosZ);
                flag = 1;
                if (HandleCylinderHit() != 0)
                    return;
            }
        }
    }
    if (IsStalled() != 0) {
        func_ov002_020edca4(this);
        return;
    }
    mdCcAc_c.Clear();
    if (flag == 1)
        mdCcAc_c.Update();
    if (o == 0)
        UpdatePos(&mdCcAc_c);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- ExecuteSeek, 0x020ecf94, size 0x34 */
/* -------------------------------------------------------------------------- */
/* State 1 (SEEK), execute: picks the variant's handler. */
void daYegg_c::ExecuteSeek() {
    if (func_ov002_020ec654())
        SeekStraight();
    else
        SeekHoming();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- EnterSeek, 0x020ecfc8, size 0x10c */
/* -------------------------------------------------------------------------- */
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* State 1 (SEEK), enter. Clears the target, plays the moving animation (file
 * 0x0210e6b0) at normal speed and sets the horizontal speed to 0x64000 (100
 * units). For the variant func_ov002_020ec654 calls zero: no gravity, a
 * target search within 0x7d0000 (2000 units) and the ground
 * collision's radius set to 0x2a000 (42 units). For the other variant: gravity -0xa000
 * (-10 units per frame squared) and a search within 0xfa0000 (4000 units). If
 * a target is found the egg turns to face it; if not, and it has already
 * targeted at least one actor, the zero variant bursts at once. Finally the
 * cylinder is re-initialised with radius 0x64000 (100 units), height 0xc8000
 * (200 units), own flags 0x202000 and hit mask 0 -- in dCc_c.h's bit table
 * that is the enemy and egg bits, with nothing able to hit it. */
void daYegg_c::EnterSeek() {
    dActor_c *r5;
    mTargetId = 0;
    mAnmModel.SetAnim((BCA_File *)data_ov002_0210e6b0.filePtr, 0, 0x1000, 0);
    mHorzSpeed = 0x64000;
    if (!func_ov002_020ec654()) {
        mVertAccel = 0;
        r5 = (dActor_c *)FindTarget(0, 0x7d0000);
        mWithMeshClsn.SetRadius(0x2a000);
    } else {
        mVertAccel = -0xa000;
        r5 = (dActor_c *)FindTarget(0, 0xfa0000);
    }
    if (r5) {
        mAngleY = Vec3_HorzAngle(&mPos, &r5->mPos);
    } else {
        if (mTargetedCount) {
            if (!func_ov002_020ec654())
                func_ov002_020edca4(this);
        }
    }
    mdCcAc_c.Init(this, 0x64000, 0xc8000, 0x202000, 0);
    mPrevAngleY = mAngleY;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- ExecuteFollowPlayer, 0x020ed0d4, size 0x4dc */
/* -------------------------------------------------------------------------- */
// obsolete provenance (this is daYegg_c state 0 execute): recovered name: daWarpkun_c_Kill
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
/* recovered: shared common types, renamed to Class_Method */
/* (obsolete provenance) daWarpkun_c::Kill - recovered from vtable slot identity */
/* State 0 (FOLLOW_PLAYER), execute. `com` is the followed daPly_c. Each call:
 *  - copies the daPly_c's 0xa4/0xa8/0xac words and its horizontal speed;
 *  - while mHoldFreezeTimer is zero (nothing in this file sets it nonzero, so every
 *    call) rebuilds the hold target and the goal angles. The target is the
 *    daPly_c's position plus the vector (0, 0, mHoldOffsetZ = -104 units)
 *    rotated by the egg's eased angles. If the daPly_c's word at +0x37c or
 *    func_ov002_020cf700 or func_ov002_020d0d2c is nonzero, the goal X/Z
 *    angles are the daPly_c's when the egg is on the ground and -0x4000 (a
 *    quarter turn) and 0 when it is not. Otherwise they are the daPly_c's, and
 *    the goal Y angle becomes the direction from the daPly_c's previous to its
 *    current position when those are at least 0x5000 (5 units) apart
 *    horizontally;
 *  - eases mPos toward the hold target (Math_Function_0203b14c: a third of
 *    the remaining distance per call, at most 1000 units) and each eased
 *    angle toward its goal (ApproachAngle: an eighth of the remaining angle,
 *    at most 0x4000 a call, at least 0x100), then copies the eased Y angle
 *    into mAngleY;
 *  - sets the animation: the moving one (file 0x0210e6b0) when mHorzSpeed is
 *    nonzero, else the idle one (0x0210eb78), at a speed of 1.0 to 2.0 that
 *    grows with mHorzSpeed and saturates at 0x20000 (32 units); the speed is
 *    0 while the daPly_c's mIsAirborne is set;
 *  - for the variant func_ov002_020ec654 calls zero, rewrites the 0xa4/0xac
 *    words as absolute values capped at 0x20000 (32 units) and tilts mAngleX
 *    (negative) and mAngleZ by up to 0xe39 (20 degrees) in proportion to
 *    them; for the other variant it runs UpdateTilt instead;
 *  - clears the cylinder, and runs the discrete ground collision unless the
 *    daPly_c's mIsNoControl is set or the daPly_c is 0x190000 (400 units) or
 *    more away. */
void daYegg_c::ExecuteFollowPlayer() {
    Vector3 in, out;
    daPly_c *com;
    int* p;
    Vector3 t1, t2;
    Vector3 v1, v2;
    Vector3 sumA, sumB, sumC;

    com = mPlayer;
    mHoldOffsetZ = -0x68000;
    p = &com->mSpeedX;
    mSpeedX = p[0];
    mVertSpeed = p[1];
    mSpeedZ = p[2];
    mHorzSpeed = com->mHorzSpeed;

    if (DecIfAbove0_Byte(&mHoldFreezeTimer) == 0) {
        in.x = 0; in.y = 0; in.z = 0;
        out.x = 0; out.y = 0; out.z = 0;
        in.z = mHoldOffsetZ;

        Matrix4x3_FromRotationZXYExt(&data_020a0e68, mEasedAngleX, mEasedAngleY, mEasedAngleZ);
        MTX_MultVec43(&in, &data_020a0e68, &out);

        if (mPlayer->mClimbClsn != 0 || func_ov002_020cf700(mPlayer) != 0 ||
            mPlayer->func_ov002_020d0d2c() != 0) {
            if (mWithMeshClsn.IsOnGround() != 0) {
                mGoalAngleX = mPlayer->mAngleX;
                mGoalAngleZ = mPlayer->mAngleZ;
                {
                    const Vector3 &a = com->mPrevPos;
                    const Vector3 &b = com->mPos;
                    t1 = a;
                    t2 = b;
                }
                Vec3_Add(&sumA, &com->mPos, &out);
                mHoldTargetX = sumA.x;
                mHoldTargetY = sumA.y;
                mHoldTargetZ = sumA.z;
            } else {
                mGoalAngleX = -0x4000;
                mGoalAngleZ = 0;
                Vec3_Add(&sumB, &com->mPos, &out);
                mHoldTargetX = sumB.x;
                mHoldTargetY = sumB.y;
                mHoldTargetZ = sumB.z;
            }
        } else {
            mGoalAngleX = com->mAngleX;
            mGoalAngleZ = com->mAngleZ;
            {
                const Vector3 &a = com->mPrevPos;
                const Vector3 &b = com->mPos;
                v1 = a;
                v2 = b;
            }
            if (Vec3_HorzDist(&v1, &v2) >= 0x5000) {
                mGoalAngleY = Vec3_HorzAngle(&v1, &v2);
            }
            Vec3_Add(&sumC, &com->mPos, &out);
            mHoldTargetX = sumC.x;
            mHoldTargetY = sumC.y;
            mHoldTargetZ = sumC.z;
        }
        mHoldFreezeTimer = 0;
    }

    {
        /* 0x1000 / 0x3000 as Fix12: a third. */
        int fd = FX_Div(0x1000, 0x3000);
        Math_Function_0203b14c(&mPosX, mHoldTargetX, fd, 0x3e8000, 4);
        Math_Function_0203b14c(&mPosY, mHoldTargetY, fd, 0x3e8000, 4);
        Math_Function_0203b14c(&mPosZ, mHoldTargetZ, fd, 0x3e8000, 4);
    }

    ApproachAngle(&mEasedAngleX, mGoalAngleX, 8, 0x4000, 0x100);
    ApproachAngle(&mEasedAngleY, mGoalAngleY, 8, 0x4000, 0x100);
    ApproachAngle(&mEasedAngleZ, mGoalAngleZ, 8, 0x4000, 0x100);

    mAngleY = mEasedAngleY;

    {
        int v98 = mHorzSpeed;
        int t = v98 >> 5;
        if (t > 0x1000) t = 0x1000;
        if (t < 0) t = 0;
        t = (t + 0x1000) >> 8;
        unsigned char byteVal = t;
        int idx = byteVal & 0x3f;
        int speed = idx << 8;
        if (mPlayer->mIsAirborne != 0) speed = 0;
        if (v98 != 0) {
            mAnmModel.SetAnim((BCA_File *)data_ov002_0210e6b0.filePtr, 0, speed, 0);
        } else {
            mAnmModel.SetAnim((BCA_File *)data_ov002_0210eb78.filePtr, 0, speed, 0);
        }
    }

    if (func_ov002_020ec654() == 0) {
        int a4 = mSpeedX;
        if (a4 < 0) a4 = -a4;
        mSpeedX = a4;
        int ac = mSpeedZ;
        if (ac < 0) ac = -ac;
        mSpeedZ = ac;
        a4 = mSpeedX;
        if (a4 > 0x20000) {
            a4 = 0x20000;
            mSpeedX = a4;
        }
        a4 = mSpeedX;
        {
            int fd1 = FX_Div(a4, 0x20000);
            int r1 = FX_MUL(fd1, 0xe39);
            short a4ang = (-r1);
            ac = mSpeedZ;
            if (ac > 0x20000) {
                ac = 0x20000;
                mSpeedZ = ac;
            }
            ac = mSpeedZ;
            {
                int fd2 = FX_Div(ac, 0x20000);
                int r2 = FX_MUL(fd2, 0xe39);
                mAngleX = a4ang;
                mAngleZ = r2;
            }
        }
    } else {
        UpdateTilt();
    }

    mdCcAc_c.Clear();
    {
        int dist = Vec3_Dist(&mPos, &com->mPos);
        if (mPlayer->mIsNoControl != 0) return;
        if (dist >= 0x190000) return;
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- EnterFollowPlayer, 0x020ed5b0, size 0x8c */
/* -------------------------------------------------------------------------- */
// State 0 (FOLLOW_PLAYER), enter. Snaps this object to the actor it follows
// (mPlayer): copies its position (mPosX/Y/Z) and Y angle (mAngleY), mirrors
// the position into mHoldTarget, and copies its rotation triple (mAngleX/Y/Z)
// into both the goal angles and the eased angles.
void daYegg_c::EnterFollowPlayer() {
    daPly_c *src;
    int* sp;
    short* m;
    src = mPlayer;
    sp = &src->mPosX;
    mPosX = sp[0];
    mPosY = sp[1];
    mPosZ = sp[2];
    src = mPlayer;
    mAngleY = src->mAngleY;
    mHoldTargetX = mPosX;
    mHoldTargetY = mPosY;
    mHoldTargetZ = mPosZ;
    src = mPlayer;
    m = &src->mAngleX;
    mGoalAngleX = m[0];
    mGoalAngleY = m[1];
    mGoalAngleZ = m[2];
    src = mPlayer;
    m = &src->mAngleX;
    mEasedAngleX = m[0];
    mEasedAngleY = m[1];
    mEasedAngleZ = m[2];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov002_020ed63c, 0x020ed63c, size 0x48 */
/* -------------------------------------------------------------------------- */
/* Switches to state `i` and runs that state's enter handler. daPly_c's
 * St_YoshiPower_Init calls it with state 1 on the egg it holds. */
void func_ov002_020ed63c(daYegg_c *c, int i) { c->mState = i; int j = c->mState; (c->*data_ov002_02110a5c[j].pmf[0])(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- RunState, 0x020ed684, size 0x48 */
/* -------------------------------------------------------------------------- */
/* Runs the current state's execute handler. */
void daYegg_c::RunState() {
    int j = mState;
    (this->*data_ov002_02110a5c[j].pmf[1])();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- IsStalled, 0x020ed6cc, size 0x6c */
/* -------------------------------------------------------------------------- */
/* recovered: shared common types */
/* Stall detector. Measures the horizontal distance from the position saved
 * on the previous call (mLastPos) to the current one, then saves the current
 * position. A move of 0x32000 (50 units) or more reloads mStallTimer to 0xf
 * and returns 0. A shorter move counts the timer down, and the function
 * returns 1 whenever the timer is zero after the countdown (first reached
 * after 15 shorter moves in a row; it stays 1 while the egg stays put), else
 * 0. */
int daYegg_c::IsStalled() {
    Fix12i d = Vec3_HorzDist((struct Vector3 *)&mLastPosX, &mPos);
    mLastPosX = mPosX;
    mLastPosY = mPosY;
    mLastPosZ = mPosZ;
    if (d < 0x32000) {
        if (DecIfAbove0_Byte(&mStallTimer) == 0)
            return 1;
    } else {
        mStallTimer = 0xf;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- UpdateTilt, 0x020ed738, size 0xc0 */
/* -------------------------------------------------------------------------- */
/* recovered: shared common types */
/* Eases the X and Z tilt (mAngleX, mAngleZ) toward a target pair, each by a
 * quarter of the remaining angle, at most 0x1000 (a 16th of a turn) a call
 * (UpdateAngle). On the ground the pair comes from the floor normal
 * (func_02010844 evaluated at the Y angle and at the Y angle minus 0x4000, a
 * quarter turn). In the air it is the daPly_c's own X and Z angles when mState
 * is 0 and there is a daPly_c, and 0 otherwise. */
void daYegg_c::UpdateTilt() {
    short e4 = 0;
    short e6 = 0;
    if (mWithMeshClsn.IsOnGround()) {
        struct Vector3 n;
        dBgPi *fr = mWithMeshClsn.GetFloorResult();
        fr->surface.CopyNormalTo(n);
        e4 = func_02010844(&n, mAngleY);
        e6 = func_02010844(&n, (mAngleY - 0x4000));
    } else {
        if (mState == 0) {
            daPly_c *p = mPlayer;
            if (p != 0) {
                e4 = p->mAngleX;
                e6 = p->mAngleZ;
            }
        }
    }
    UpdateAngle(mAngleX, e4, 4, 0x1000);
    UpdateAngle(mAngleZ, e6, 4, 0x1000);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- UpdateShadow, 0x020ed7f8, size 0x1a0 */
/* -------------------------------------------------------------------------- */
/* Drop shadow, called from Behavior. Skipped when the followed daPly_c's
 * mOpacity is below 1. It probes the floor with a dBgCh_Gnd from 0x28000 (40
 * units) above the egg (its +0x44 is the floor height, dBgCh_Gnd's clsnY);
 * r5 is the egg's height over that floor when one was found (the probe's start
 * point otherwise), at least 0x1000 (1 unit), and r4 the
 * shadow size, 0x50000 (80 units) less 3/32 of r5 and never below 0xa000 (10
 * units). It fills mShadowMtx with a Y rotation and the position >> 3, then
 * draws the shadow, cylinder or cuboid by the variant bit, unless the egg's
 * flags have 0x40000 (dActor_c.h lists 0x020000 / 0x040000 as yoshi-mouth
 * states written by actor code), or the daPly_c's word at +0x37c is nonzero, or either of
 * func_ov002_020cf700 / func_ov002_020d0d2c says so for the daPly_c. */
void daYegg_c::UpdateShadow() {
    Vector3 v;
    int r5;
    int r4;
    int b;

    if (mPlayer->mOpacity < 1)
        return;

    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y += 0x28000;
    dBgCh_Gnd rg;
    rg.SetObjAndPos(v, 0);
    r4 = v.y;
    if (rg.DetectClsn())
        r4 = rg.clsnY;
    r5 = mPosY - r4;
    if (r5 <= 0x1000) r5 = 0x1000;
    r4 = 0x50000 - FX_MUL(r5, 0x180);
    if (r4 < 0xa000) r4 = 0xa000;
    Matrix4x3_FromRotationY(mShadowMtx, mAngleY);
    mShadowMtx[9] = mPosX >> 3;
    mShadowMtx[10] = mPosY >> 3;
    mShadowMtx[11] = mPosZ >> 3;
    b = (mFlags & 0x40000) ? 1 : 0;
    if (b == 0 && mPlayer->mClimbClsn == 0 && !func_ov002_020cf700(mPlayer) &&
        !mPlayer->func_ov002_020d0d2c()) {
        if (func_ov002_020ec654() == 0) {
            DropShadowRadHeight(mShadowModel, *(Matrix4x3 *)mShadowMtx, r4, r5 + 0x28000, 0xf);
        } else {
            DropShadowScaleXYZ(mShadowModel, *(Matrix4x3 *)mShadowMtx, r4, r5 + 0x28000, r4, 0xf);
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- UpdateModelMatrix, 0x020ed998, size 0x1a4 */
/* -------------------------------------------------------------------------- */
/* Places the egg's model matrix, called from Behavior. With
 * daYegg_FLAG_ATTACHED set and a daPly_c, it copies the 12-word matrix
 * pointed to by the daPly_c's mCarryMatrix, shifts
 * its translation by (0x3f, 9, 0xb) and applies a ZXY rotation of
 * (-0x49f5, -0xc17, -0x293f), then moves the egg's mPos to the resulting
 * translation << 3 and returns; matrix units are mPos >> 3 throughout.
 * Otherwise it builds the matrix from the egg's own pose: the translation is
 * mPos with 0x14000 (20 units) added to Y, scaled by 1/8 (x << 9 >> 12, rounded),
 * rotated by (mAngleX, mAngleY, mAngleZ), and stored into the model's own
 * matrix (mAnmModel.mat4x3). */
void daYegg_c::UpdateModelMatrix() {
    int on = (mFlags & daYegg_FLAG_ATTACHED) != 0;
    if (on && mPlayer) {
        Vector3 v(0, 0, 0);
        *(Mtx43 *)&data_020a0e68 = *(Mtx43 *)mPlayer->mCarryMatrix;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x3f, 9, 0xb);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, 0xffffb60b, 0xfffff3e9, 0xffffd6c1);
        v.x = ((Mtx43 *)&data_020a0e68)->r[3].x;
        v.y = ((Mtx43 *)&data_020a0e68)->r[3].y;
        v.z = ((Mtx43 *)&data_020a0e68)->r[3].z;
        mPosX = v.x << 3;
        mPosY = v.y << 3;
        mPosZ = v.z << 3;
        return;
    }
    Matrix4x3_FromTranslation(&data_020a0e68, ((((long long)mPosX << 9) + 0x800) >> 12),
                              FX_MUL(mPosY + 0x14000, 0x200),
                              ((((long long)mPosZ << 9) + 0x800) >> 12));
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        mAngleX, mAngleY, mAngleZ);
    *(Mtx43 *)&mAnmModel.mat4x3 = *(Mtx43 *)&data_020a0e68;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- FindTarget, 0x020edb3c, size 0x168 */
/* -------------------------------------------------------------------------- */
/* Target search. If five actors have already been targeted it bursts the egg
 * (for the variant func_ov002_020ec654 calls zero) and returns 0. Otherwise it
 * walks the actor list for the nearest actor, closer than `best` (the
 * caller's range), that is not the egg or the followed daPly_c, has profile
 * bit 0x10000000 and not the off-screen bit 0x8, is not in mTargetedIds and,
 * for the other variant, answers nonzero to vtable slot 20. The winner's
 * uniqueID is appended to mTargetedIds, becomes mTargetId and bumps
 * mTargetedCount; the winner is returned, or 0 if none qualified. `a1` is
 * not used. */
int daYegg_c::FindTarget(int a1, int best) {
    dActor_c *found;
    dActor_c *actor;
    int matched;
    int b;
    struct P { int a, b, c, d; };
    volatile struct P _p;
    struct P *_q = (struct P *)&_p;

    if (mTargetedCount >= 5) {
        if (func_ov002_020ec654() == 0) {
            func_ov002_020edca4(this);
        }
        return 0;
    }

    found = 0;
    for (actor = dActor_c::Next(0); actor != 0; actor = dActor_c::Next(actor)) {
        if (actor == this)
            continue;
        if (actor == (dActor_c *)mPlayer)
            continue;
        b = (actor->mFlags & 0x10000000) != 0;
        if (!b)
            continue;
        b = (actor->mFlags & 8) != 0;
        if (b)
            continue;
        matched = 0;
        for (int i = 0; i < 5; i++) {
            int bv = mTargetedIds[i];
            if (bv == actor->uniqueID)
                matched = 1;
        }
        if (matched != 0)
            continue;
        if (func_ov002_020ec654() != 0) {
            if (actor->Virtual50() == 0)
                continue;
        }
        {
            int d = Vec3_Dist(&mPos, &actor->mPos);
            if (d < best) {
                best = d;
                found = actor;
            }
        }
    }
    if (found != 0) {
        int idx = mTargetedCount;
        mTargetedIds[idx] = found->uniqueID;
        mTargetId = found->uniqueID;
        mTargetedCount += 1;
    }
    return (int)found;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov002_020edca4, 0x020edca4, size 0x120 */
/* -------------------------------------------------------------------------- */
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* The burst. Does nothing if mBurstDone is already set. Otherwise it pays out
 * (PayOut), and when the star bit is set gives up the tracked
 * star slot and spawns a SILVER_STAR (param1 0x10) 40 units above the egg,
 * which SeekNewMarker then takes together with the daPly_c. It then
 * starts four particle effects (ids 0x3f to 0x42) at the egg, marks the egg
 * for destruction, plays Sound::PlayCharVoice id 0x103 at its camera-space
 * position and sets mBurstDone. Stays a free function: d_a_ply_methods.cpp calls it
 * on its held object, and daPly_c is not deslopped here. */
void func_ov002_020edca4(daYegg_c* self)
{
    struct Vector3 pos;
    char* spawned;
    int xv, zv, yv, sy;

    if (self->mBurstDone != 0) return;

    self->PayOut();

    yv = self->mPosY;
    zv = self->mPosZ;
    sy = yv + 0x28000;
    xv = self->mPosX;
    pos.x = xv;
    pos.y = sy;
    pos.z = zv;
    if (self->HasStarPayout() != 0) {
        self->UntrackStar(*(s8 *)&self->mStarSlot);

        spawned =
            (char *)dActor_c::Spawn(ACTOR_SILVER_STAR, 0x10, pos, 0, self->mPlayer->mAreaId, -1);
        if (spawned != 0) {
            ((daStar_c *)spawned)->SeekNewMarker((char *)self->mPlayer, 1);
        }
    }

    Particle::System::NewSimple(kPtclYoshiEggBreakA, self->mPosX, self->mPosY, self->mPosZ);
    Particle::System::NewSimple(kPtclYoshiEggBreakB, self->mPosX, self->mPosY, self->mPosZ);
    Particle::System::NewSimple(kPtclYoshiEggBreakC, self->mPosX, self->mPosY, self->mPosZ);
    Particle::System::NewSimple(kPtclYoshiEggBreakD, self->mPosX, self->mPosY, self->mPosZ);
    self->MarkForDestruction();
    Sound::PlayCharVoice(0, 0x103, self->mCamSpacePos);

    self->mBurstDone = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- HandleCylinderHit, 0x020eddc4, size 0x190 */
/* -------------------------------------------------------------------------- */
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* HandleCylinderHit at 0x020eddc4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
/* Contact handling, run each frame the egg is flying. The actor that touched
 * the egg's cylinder is looked up from the cylinder's otherOwner
 * (mdCcAc_c.otherOwner, +0x134); the answer is 0 and nothing happens if
 * there is none or it is the followed daPly_c. When it is some other actor of
 * type PLAYER (any daPly_c-type actor except the followed one):
 * if the cylinder's hit flags (mdCcAc_c.hitFlags, +0x130) have bit 0x8000 the
 * egg enters state 3 and returns 1; if none of the bits 0x26fe0 are set, the
 * daPly_c is hurt (daPly_c::Hurt at the egg's position with 1, 12 units in
 * Fix12, 1, 0, 1). When the touched actor is one of the six burst actors
 * (BOMBKING, HOLHEI, KURIKING, KURIBO_L, WANWAN, WANWAN2) the egg bursts and
 * returns 1. When it is the actor currently sought (mTargetId) the egg pays
 * out, re-enters state 1 and returns 1. Any other case returns 0. */
int daYegg_c::HandleCylinderHit() {
    daPly_c *actor;
    struct dActor_c* other;
    u32 id;
    u32 flags;
    u16 type;

    id = mdCcAc_c.otherOwner;
    if (id != 0) {
        actor = static_cast<daPly_c *>(dActor_c::FindWithID(id));
        if (actor != 0 && actor != (struct dActor_c *)mPlayer) {
            {
                int t = (int)(actor->actorID == ACTOR_PLAYER);
                if (t != 0) {
                    flags = mdCcAc_c.hitFlags;
                    if (flags & 0x8000) {
                        func_ov002_020ed63c(this, daYegg_STATE_DROP);
                        return 1;
                    }
                    if (!(flags & 0x26fe0)) {
                        struct Vector3 pos;
                        pos.x = mPosX;
                        pos.y = mPosY;
                        pos.z = mPosZ;
                        actor->Hurt(pos, 1, 0xc000, 1, 0, 1);
                    }
                }
            }

            type = actor->actorID;
            switch (type) {
            case ACTOR_BOMBKING:
            case ACTOR_HOLHEI:
            case ACTOR_KURIKING:
            case ACTOR_KURIBO_L:
            case ACTOR_WANWAN:
            case ACTOR_WANWAN2:
                func_ov002_020edca4(this);
                return 1;
            }

            other = 0;
            id = mTargetId;
            if (id != 0) {
                other = dActor_c::FindWithID(id);
            }
            if (other == actor) {
                PayOut();
                func_ov002_020ed63c(this, daYegg_STATE_SEEK);
                return 1;
            }
        }
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method */
/* Releases the two shared animation files and, if the egg pays a blue coin,
   unloads the blue-coin model InitResources loaded. */
int daYegg_c::CleanupResources()
{
    data_ov002_0210e6b0.Release();
    data_ov002_0210eb78.Release();
    if (HasBlueCoinPayout() != 0)
        UnloadBlueCoinModel();
    return 1;
}

/* recovered: named members + shared header, real C++ method -- vtable slot 9 */
/* Draws the egg's model unless it is hidden: skipped when mFlags has 0x40000,
   when the followed daPly_c is inside a cannon, or when the daPly_c's opacity
   is below 1. */
int daYegg_c::Render()
{
    /* The temporary is load-bearing and must not be folded into the `if`, the
       same way it is in daBakubaku_c::Render: `if (mFlags & 0x40000)` tests the
       masked word directly, while the ROM materialises the 0/1 first. Folding it
       changes the function's SIZE, which is what a `999 word(s) differ` says. */
    int b = ((mFlags & 0x40000) != 0);
    if (b) return 1;

    if (mPlayer->IsInsideOfCannon()) return 1;
    if (mPlayer->mOpacity < 1) return 1;
    mAnmModel.Render(0);
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* Per-frame update: runs the current state's execute handler and advances the
 * model animation. In every state but 1 the egg takes the followed daPly_c's
 * opacity; while that opacity is below 10 it is also pinned to the daPly_c's
 * position plus the vector (0, 0, -104 units) rotated by the daPly_c's angles.
 * In state 1 it is fully opaque (31). Then it places the model matrix
 * (UpdateModelMatrix), draws the drop shadow (UpdateShadow), moves to
 * the next payout slot once the current one is paid, and bursts when all five
 * slots are used. */
int daYegg_c::Behavior()
{
    Vector3 vin;
    Vector3 vmid;
    Vector3 vout;
    RunState();
    mAnmModel.Advance();
    if (mState != daYegg_STATE_SEEK) {
        if (mPlayer->mOpacity < 0xa) {
            /* BOTH oddities below are load-bearing, measured one at a time.

               `ang` must stay a pointer: reading the three angles as
               mPlayer->mAngleX/Y/Z instead reloads mPlayer per field and
               changes the function's size.

               `vin.z` really is written twice. Dropping the dead first store
               also changes the size, so the ROM's own source had it. */
            s16 *ang;
            vin.z = 0;
            vin.z = -0x68000;
            vin.x = 0;
            vin.y = 0;
            vmid.x = 0;
            vmid.y = 0;
            vmid.z = 0;
            ang = &mPlayer->mAngleX;
            Matrix4x3_FromRotationZXYExt(&data_020a0e68, ang[0], ang[1], ang[2]);
            MTX_MultVec43(&vin, &data_020a0e68, &vmid);
            Vec3_Add(&vout, &mPlayer->mPos, &vmid);
            mPosX = vout.x;
            mPosY = vout.y;
            mPosZ = vout.z;
        }
        mAnmModel.ApplyOpacity(mPlayer->mOpacity, 0);
    } else {
        mAnmModel.ApplyOpacity(0x1f, 0);
    }
    UpdateModelMatrix();
    UpdateShadow();
    if (mPayoutDone[mPayoutIdx] != 0)
        mPayoutIdx++;
    if (mPayoutIdx >= 5) {
        func_ov002_020edca4(this);
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Vtable slot 0. This was the last daYegg_c function still written as a C free
 * function reaching every field through a raw offset -- it named not one member,
 * which is why it could not even include its own header once daYegg_c became a
 * real dEnemyBase_c subclass.
 *
 * Sets up the egg: mParamHigh from param1; the model by the variant
 * (func_ov002_020ec654 picks which of the two models at data_ov002_021000a0
 * and whether the shadow is a cylinder or a cuboid); the idle animation;
 * gravity -0x2000 (-2 units per frame squared) and a terminal velocity of
 * -0x3c000 (-60 units per frame); and the starting state from param1 & 3. The
 * same two bits choose the size: states 0 and 1 are normal size (scale 1.0,
 * collision cylinder 0x46000 by 0x8c000, i.e. 70 by 140 units, ground-collision
 * radius and height 0x28000 = 40 units), state 2 is the big egg (scale 2.0,
 * 0x78000 by 0xa0000 = 120 by 160 units, ground-collision radius and height
 * 0x64000 = 100 units), state 3 gets neither.
 * The cylinder's own flags 0x200002 and hit mask 0xa08000 are left as raw
 * numbers. It then clears the targeted-ID list, seeds both angle triples from
 * the actor's own angles, arms mStallTimer, loads the blue-coin model if the
 * egg pays one and tracks a star slot if it pays the star. */
int daYegg_c::InitResources()
{
    int idx;
    int i;

    mParamHigh = (param1 >> 4);

    idx = 0;
    if (func_ov002_020ec654() != 0)
        idx = 1;

    dExtFrameCtrl_c::LoadFile(data_ov002_0210e6b0);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210eb78);
    if (mAnmModel.SetFile(*(BMD_File **)(data_ov002_021000a0[idx] + 4), 1, -1) == 0)
        return 0;

    /* The predicate is asked a SECOND time rather than reusing idx: the ROM calls
       0x020ec654 twice, and folding it into the index above loses a bl. */
    if (func_ov002_020ec654() == 0) {
        if (mShadowModel.InitCylinder() == 0)
            return 0;
    } else {
        if (mShadowModel.InitCuboid() == 0)
            return 0;
    }

    mAnmModel.SetAnim((BCA_File *)data_ov002_0210eb78.filePtr, 0, 0x1000, 0);

    mAreaId = -1;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mState = param1 & 3;

    switch (mState) {
    case 0:
    case 1:
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
        mdCcAc_c.Init(this, 0x46000, 0x8c000, 0x200002, 0xa08000);
        mWithMeshClsn.Init(this, 0x28000, 0x28000, 0, 0);
        break;
    case 2:
        mScaleX = 0x2000;
        mScaleY = 0x2000;
        mScaleZ = 0x2000;
        mdCcAc_c.Init(this, 0x78000, 0xa0000, 0x200002, 0xa08000);
        mWithMeshClsn.Init(this, 0x64000, 0x64000, 0, 0);
        break;
    default:
        break;
    }

    for (i = 0; i < 5; i++)
        mTargetedIds[i] = 0;

    mEasedAngleX = mAngleX;
    mEasedAngleY = mAngleY;
    mEasedAngleZ = mAngleZ;
    mGoalAngleX = mAngleX;
    mGoalAngleY = mAngleY;
    mGoalAngleZ = mAngleZ;
    mStallTimer = 0xf;

    if (HasBlueCoinPayout() != 0)
        LoadBlueCoinModel();
    if (HasStarPayout() != 0)
        mStarSlot = TrackStar(0, 1);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- daYegg_c_classInit, 0x020ee3d8, size 0x54 */
/* -------------------------------------------------------------------------- */
/* recovered: vtable identified, globals resolved */
/* Reconstructed source-style name: SM64DS proves daYegg_c through RTTI,
 * allocation size, vtable identity, and the YOSHI_EGG registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: YoshiEgg_Spawn.
 *
 * `new daYegg_c` is the whole sequence the loose factory spelled by hand:
 * fBase_c::operator new(0x42c), dEnemyBase_c's base constructor, the vptr
 * store, then the four member constructors in declaration order. */
daYegg_c *daYegg_c_classInit(void)
{
    return new daYegg_c;
}

/* The state table func_ov002_020ed63c / RunState dispatch
   through: two member-function pointers per state, enter then execute.
   Retail stores the records as compiler temporaries in .data
   (0x0210ad38..0x0210ad78) and the live table in .bss. */
Entry data_ov002_02110a5c[4] = {
    {&daYegg_c::EnterFollowPlayer, &daYegg_c::ExecuteFollowPlayer},
    {&daYegg_c::EnterSeek, &daYegg_c::ExecuteSeek},
    {&daYegg_c::EnterWobble, &daYegg_c::ExecuteWobble},
    {&daYegg_c::EnterDrop, &daYegg_c::ExecuteDrop},
};
