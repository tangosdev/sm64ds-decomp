//cpp
/* ov002/daObjFire_c -- reconstructed translation unit (8 functions).
 *
 * ROM run 0x020b5734..0x020b5a18, plus the class's .data run at
 * 0x02108edc..0x02108fb4 (_ZTI, _ZTS, the two spawn-info records and _ZTV).
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the reverse of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * Members, in ROM address order (each was a one-function shard before this
 * TU took over the run; the shards are gone):
 *   [0] 0x020b5734  daObjFire_c::~daObjFire_c   (D1)
 *   [1] 0x020b5764  daObjFire_c::~daObjFire_c   (D0)
 *   [2] 0x020b57a8  daObjFire_c::Behavior
 *   [3] 0x020b5940  daObjFire_c::InitResources
 *   [4] 0x020b599c  daObjFire_c::OnTurnIntoEgg
 *   [5] 0x020b59a0  daObjFire_c::OnYoshiTryEat
 *   [6] 0x020b59a8  daObjFire_c_classInit_OBJ_BLUE_FIRE  (factory)
 *   [7] 0x020b59e0  daObjFire_c_classInit_OBJ_RED_FIRE   (factory)
 */

#include "daObjFire_c.h"
#include "Player.h"
#include "decl_common.h"  /* declares _ZTV11daObjFire_c */

extern "C" {
u8 DecIfAbove0_Byte(u8 *value);
u32 func_020228dc(Fix12i x, Fix12i y, Fix12i z);
u32 func_0202293c(Fix12i x, Fix12i y, Fix12i z);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
u32 id, Fix12i x, Fix12i y, Fix12i z);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj( void *self, dActor_c *actor, Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjFire_c_classInit_OBJ_RED_FIRE, 0x020b59e0, size 0x38                      */
/* ROM ordinal 6 -- daObjFire_c_classInit_OBJ_BLUE_FIRE, 0x020b59a8, size 0x38                     */
/* -------------------------------------------------------------------------- */
/* ONE class, TWO profiles. OBJ_RED_FIRE (actor 316) and OBJ_BLUE_FIRE (actor
 * 317) each own a descriptor and a byte-identical factory, and both descriptors
 * install the same vtable at 0x02108f38 and the same 0x118 allocation. Behavior()
 * tells them apart at run time by actorID == 0x13d.
 *
 * The EAD convention would name both factories daObjFire_c_classInit, and two
 * C-linkage definitions cannot share one name, so each carries its profile's
 * suffix -- the spellings the ROM's own debug string table proves: OBJ_RED_FIRE
 * at 0x020904b8, OBJ_BLUE_FIRE at 0x020905d8. Both are reconstructed spellings,
 * not recovered symbols. */
extern "C" {
extern void *_ZN7fBase_cnwEj(u32 size);
extern void _ZN8dActor_cC2Ev(void *self);
extern void _ZN7dCcAc_cC1Ev(void *self);

int *daObjFire_c_classInit_OBJ_RED_FIRE(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(280);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        /* &[2], not the bare symbol: this TU EMITS the vtable, so mwcc's symbol is
         * the object start at 0x02108f30 and +8 is what reaches the 0x02108f38
         * address point. The addend-0 spelling is right only for a TU that imports
         * its vtable, which is what the retired one-function factories did. */
        p[0] = (int)&_ZTV11daObjFire_c[2];
        _ZN7dCcAc_cC1Ev((char *)p + 0xe4);
    }
    return p;
}

int *daObjFire_c_classInit_OBJ_BLUE_FIRE(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(280);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV11daObjFire_c[2];
        _ZN7dCcAc_cC1Ev((char *)p + 0xe4);
    }
    return p;
}
}

struct FireSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;                 /* 0x1000 == 1.0 */
    s32 clipDistance;
    s32 farDistance;
};
typedef char FireSpawnInfo_size_must_be_0x1c[sizeof(FireSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_OBJ_RED_FIRE
extern "C" FireSpawnInfo g_profile_OBJ_RED_FIRE = {
    daObjFire_c_classInit_OBJ_RED_FIRE, 0x013c, 0x013a, 0x00800003,
    0x00000000, 0x00100000, 0x01000000, 0x00000000
};

// @symbol g_profile_OBJ_BLUE_FIRE
extern "C" FireSpawnInfo g_profile_OBJ_BLUE_FIRE = {
    daObjFire_c_classInit_OBJ_BLUE_FIRE, 0x013d, 0x013b, 0x00800003,
    0x00000000, 0x00100000, 0x02000000, 0x00000000
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN11daObjFire_c13OnYoshiTryEatEv, 0x020b59a0, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjFire_c13OnYoshiTryEatEv
/* recovered: renamed to Class_Method */
s32 daObjFire_c::OnYoshiTryEat() {
    return 5;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN11daObjFire_c13OnTurnIntoEggER6Player, 0x020b599c, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjFire_c13OnTurnIntoEggER6Player
// recovered name: BlueFlame_OnTurnIntoEgg
/* daObjFire_c::OnTurnIntoEgg -- vtable slot 19, verified against ov002
 * relocs.txt: _ZTV11daObjFire_c (0x02108f38) + 0x4c -> 0x020b599c, exactly this
 * placeholder's former address (former name func_ov002_020b599c). The ROM
 * body is a single `bx lr` -- an intentional no-op override, not a stub.
 * Matched byte-for-byte with mwccarm 2004/b56 (ov002).
 */
int daObjFire_c::OnTurnIntoEgg(Player &player)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN11daObjFire_c13InitResourcesEv, 0x020b5940, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjFire_c13InitResourcesEv
int daObjFire_c::InitResources()
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, 0x41000, 0x41000, 0x200002, 0x8000);
    mFlamePos.x = mPosX;
    mFlamePos.y = mPosY;
    mFlamePos.z = mPosZ;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN11daObjFire_c8BehaviorEv, 0x020b57a8, size 0x198 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjFire_c8BehaviorEv
int daObjFire_c::Behavior()
{
    struct Vector3 pos;
    struct Vector3 particlePos;
    struct Vector3 dust;

    pos.x = mFlamePos.x;
    pos.y = mFlamePos.y;
    pos.z = mFlamePos.z;
    pos.y += 0x32000;

    if (DecIfAbove0_Byte(&mDisappearTimer) != 0) {
        mdCcAc_c.Clear();
        if (mDisappearTimer == 1) {
            ((int *)&dust)[0] = ((int *)&pos)[0];
            ((int *)&dust)[1] = ((int *)&pos)[1];
            ((int *)&dust)[2] = ((int *)&pos)[2];
            DisappearPoofDustAt(dust);
        }
        return 1;
    }

    mPosX = mFlamePos.x;
    mPosY = mFlamePos.y;
    mPosZ = mFlamePos.z;
    mFlags &= ~0xc0000;

    if ((int)(actorID == 0x13d) != 0)
        func_020228dc(pos.x, pos.y, pos.z);
    else
        func_0202293c(pos.x, pos.y, pos.z);

    if ((int)((mFlags & 0x20000) != 0) != 0) {
        mDisappearTimer = 0x1e;
        particlePos.x = mFlamePos.x;
        particlePos.y = mFlamePos.y;
        particlePos.z = mFlamePos.z;
        particlePos.y -= 0x1b000;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
            0x9f, ((int *)&particlePos)[0], particlePos.y, particlePos.z);
    }

    {
        int id = mdCcAc_c.otherOwner;
        if (id != 0) {
            dActor_c *actor = FindWithID(id);
            if (actor != 0) {
                if ((int)(actor->actorID == 0xbf) != 0) {
                    if ((mdCcAc_c.hitFlags & 0x8000) == 0) {
                        ((Player *)actor)->Burn();
                    }
                }
            }
        }
    }

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN11daObjFire_cD0Ev, 0x020b5764, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjFire_cD0Ev
/* Deleting destructor, vtable slot 17. No body here: the destructor is inline in
 * include/daObjFire_c.h, which is what makes mwccarm emit exactly the retail
 * D1/D0 pair in retail order and no D2. dActor_c's inline operator delete
 * supplies the actor-heap release after complete destruction. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN11daObjFire_cD1Ev, 0x020b5734, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjFire_cD1Ev
/* Complete destructor, vtable slot 16. No body here either -- same inline header
 * definition. The only member with a destructor is the dCcAc_c collider, and it
 * is destroyed before the dActor_c base. */
