#ifndef DAOBJICEBLOCK_C_H
#define DAOBJICEBLOCK_C_H

#include "types.h"
#include "dBgActor_c.h"
#include "dCcAc_c.h"

struct Player;

/* Ice block (profile ICE_BLOCK_LL). Fire melts it -- it shrinks over 30
 * frames and is gone -- and an explosion or a mega character shatters it;
 * either way it lets go of the star (actor 0xb2) frozen inside.
 * ROM RTTI daObjIceBlock_c; direct base dBgActor_c. Factory allocates 0x368,
 * and mContainedActor closes exactly on it.
 *
 * The vtable was diffed slot by slot against _ZTV10dBgActor_c; only the slots
 * declared below differ, every other slot is the base's own word. Slot 27
 * (OnHitByMegaChar) and slot 31 (Kill, dBgActor_c's own new virtual) are
 * overrides; the class adds no slot of its own.
 *
 * daObjIceBlock_c_classInit is reconstructed (RTTI daObjIceBlock_c,
 * ICE_BLOCK_LL registry). Retail does not store that spelling. Historical
 * aliases: IceBlock (the class), IceBlock_Spawn.
 */
struct daObjIceBlock_c : dBgActor_c {
    dCcAc_c mCollider;            /* 0x320 -- the cylinder fire and explosions hit */
    u8  mMeltTimer;               /* 0x354 -- 0x1e on a fire hit; counts the melt down to destruction */
    s32 mScale;                   /* 0x358 -- 1.0 until melting, then mMeltTimer / 30 */
    u32 mParticleHandle1;         /* 0x35c -- recycled Particle::System handle, effect 0x77 */
    u32 mParticleHandle2;         /* 0x360 -- the same, effect 0x78 */
    dActor_c *mContainedActor;    /* 0x364 -- the actor frozen inside (actor 0xb2), or null */

    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daObjIceBlock_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnHitByMegaChar(Player &player);   /* slot 27 */
    virtual void Kill();                            /* slot 31 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjIceBlock_c_size_must_be_0x368[
    sizeof(daObjIceBlock_c) == 0x368 ? 1 : -1];
#endif

#endif /* DAOBJICEBLOCK_C_H */
