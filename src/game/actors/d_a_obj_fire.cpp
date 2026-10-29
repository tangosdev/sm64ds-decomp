//cpp
/* ov002/daObjFire_c -- red and blue flames.
 *
 * RTTI names daObjFire_c. overlay_actors.md lists RED_FLAME(316) and
 * BLUE_FLAME(317); the debug table names those profiles OBJ_RED_FIRE
 * and OBJ_BLUE_FIRE. POPPING_LAVA_BUBBLES(196) is daObjLava_c.
 *
 * One class, two C-linkage factories: EAD would name both
 * daObjFire_c_classInit (not_apply=global_classinit_name_collision).
 *
 * S41: RTTI daObjFire_c; D1 0x020b5734 size 0x30 (slot 16),
 * D0 0x020b5764 size 0x44 (slot 17); inline dtor in daObjFire_c.h
 * emits D1 then D0, no D2.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 *
 * func_020228dc / func_0202293c are the arm9 blue/red flame particle
 * wrappers (effects 0x117 / 0x118). Their names are not recovered and no
 * header declares them.
 */

#include "daObjFire_c.h"
#include "SpawnInfo.h"
#include "Player.h"
#include "Particle__System.h"

extern "C" {
u8 DecIfAbove0_Byte(u8 *value);
u32 func_020228dc(Fix12i x, Fix12i y, Fix12i z);
u32 func_0202293c(Fix12i x, Fix12i y, Fix12i z);
}

/* Actor ID 0x13d is BLUE_FLAME, 0xbf is PLAYER. dActor_c::mFlags bit
 * 0x20000 is the first of the two yoshi-mouth states, set while Yoshi is
 * eating the actor. These int-returning predicates are what make the ROM materialize 0/1 before
 * testing it. */
static inline int IsBlueFlame(dActor_c *actor) {
    return actor->actorID == 0x13d;
}

static inline int IsBeingEaten(dActor_c *actor) {
    return (actor->mFlags & 0x20000) != 0;
}

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

// @symbol daObjFire_c_classInit_OBJ_RED_FIRE
extern "C" daObjFire_c *daObjFire_c_classInit_OBJ_RED_FIRE() {
    return new daObjFire_c();
}

// @symbol daObjFire_c_classInit_OBJ_BLUE_FIRE
extern "C" daObjFire_c *daObjFire_c_classInit_OBJ_BLUE_FIRE() {
    return new daObjFire_c();
}

// @symbol g_profile_OBJ_RED_FIRE
extern "C" SpawnInfo<daObjFire_c> g_profile_OBJ_RED_FIRE = {
    daObjFire_c_classInit_OBJ_RED_FIRE,
    0x013c,
    0x013a,
    0x00800003,
    0x00000000,
    0x00100000,
    0x01000000,
    0x00000000,
};

// @symbol g_profile_OBJ_BLUE_FIRE
extern "C" SpawnInfo<daObjFire_c> g_profile_OBJ_BLUE_FIRE = {
    daObjFire_c_classInit_OBJ_BLUE_FIRE,
    0x013d,
    0x013b,
    0x00800003,
    0x00000000,
    0x00100000,
    0x02000000,
    0x00000000,
};

// @symbol _ZN11daObjFire_c13OnYoshiTryEatEv
s32 daObjFire_c::OnYoshiTryEat() {
    return 5;
}

// @symbol _ZN11daObjFire_c13OnTurnIntoEggER6Player
void daObjFire_c::OnTurnIntoEgg(Player &player) {}

// @symbol _ZN11daObjFire_c13InitResourcesEv
int daObjFire_c::InitResources() {
    mdCcAc_c.Init(this, 0x41000, 0x41000, 0x200002, 0x8000);
    mFlamePos.x = mPosX;
    mFlamePos.y = mPosY;
    mFlamePos.z = mPosZ;
    return 1;
}

// @symbol _ZN11daObjFire_c8BehaviorEv
int daObjFire_c::Behavior() {
    Vector3 pos;
    Vector3 particlePos;
    Vector3 dust;

    pos.x = mFlamePos.x;
    pos.y = mFlamePos.y;
    pos.z = mFlamePos.z;
    pos.y += 0x32000;

    if (DecIfAbove0_Byte(&mDisappearTimer) != 0) {
        mdCcAc_c.Clear();
        if (mDisappearTimer == 1) {
            dust.x = pos.x;
            dust.y = pos.y;
            dust.z = pos.z;
            DisappearPoofDustAt(dust);
        }
        return 1;
    }

    mPosX = mFlamePos.x;
    mPosY = mFlamePos.y;
    mPosZ = mFlamePos.z;
    mFlags &= ~0xc0000;

    if (IsBlueFlame(this))
        func_020228dc(pos.x, pos.y, pos.z);
    else
        func_0202293c(pos.x, pos.y, pos.z);

    if (IsBeingEaten(this)) {
        mDisappearTimer = 0x1e;
        particlePos.x = mFlamePos.x;
        particlePos.y = mFlamePos.y;
        particlePos.z = mFlamePos.z;
        particlePos.y -= 0x1b000;
        Particle::System::NewSimple(0x9f, particlePos.x, particlePos.y, particlePos.z);
    }

    {
        int id = mdCcAc_c.otherOwner;
        if (id != 0) {
            dActor_c *actor = FindWithID(id);
            if (actor != 0) {
                if (IsPlayerActor(actor)) {
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
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjFire_cD0Ev
/* Deleting destructor, vtable slot 17. No body here: the destructor is inline in
 * include/daObjFire_c.h, which is what makes mwccarm emit exactly the retail
 * D1/D0 pair in retail order and no D2. dActor_c's inline operator delete
 * supplies the actor-heap release after complete destruction. */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjFire_cD1Ev
/* Complete destructor, vtable slot 16. No body here either -- same inline header
 * definition. The only member with a destructor is the dCcAc_c collider, and it
 * is destroyed before the dActor_c base. */
