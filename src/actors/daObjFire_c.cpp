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
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 *
 * deslop leftovers:
 * - dCcAc_c::Init 6az: InitResources passes Fix12<int> by value; the
 *   header method form size-DIFFs.
 * - func_020228dc / func_0202293c: blue/red flame particle wrappers
 *   (effects 0x117 / 0x118). No recovered names.
 * - Particle::System::NewSimple stays mangled (Fix12i by value, 6az).
 * - Player::Burn stays mangled (no Player.h).
 */

#include "daObjFire_c.h"

extern "C" {
u8 DecIfAbove0_Byte(u8 *value);
u32 func_020228dc(Fix12i x, Fix12i y, Fix12i z);
u32 func_0202293c(Fix12i x, Fix12i y, Fix12i z);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *self, dActor_c *actor, Fix12i radius, Fix12i height,
    u32 flags, u32 vulnFlags);
void _ZN6Player4BurnEv(void *player);
}

// @symbol daObjFire_c_classInit_OBJ_RED_FIRE
extern "C" daObjFire_c *daObjFire_c_classInit_OBJ_RED_FIRE()
{
    return new daObjFire_c();
}

// @symbol daObjFire_c_classInit_OBJ_BLUE_FIRE
extern "C" daObjFire_c *daObjFire_c_classInit_OBJ_BLUE_FIRE()
{
    return new daObjFire_c();
}

struct FireSpawnInfo {
    daObjFire_c *(*classInit)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
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

// @symbol _ZN11daObjFire_c13OnYoshiTryEatEv
s32 daObjFire_c::OnYoshiTryEat()
{
    return 5;
}

// @symbol _ZN11daObjFire_c13OnTurnIntoEggER6Player
void daObjFire_c::OnTurnIntoEgg(Player &player)
{
}

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

// @symbol _ZN11daObjFire_c8BehaviorEv
int daObjFire_c::Behavior()
{
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
            0x9f, particlePos.x, particlePos.y, particlePos.z);
    }

    {
        int id = mdCcAc_c.otherOwner;
        if (id != 0) {
            dActor_c *actor = FindWithID(id);
            if (actor != 0) {
                if ((int)(actor->actorID == 0xbf) != 0) {
                    if ((mdCcAc_c.hitFlags & 0x8000) == 0) {
                        _ZN6Player4BurnEv(actor);
                    }
                }
            }
        }
    }

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}
