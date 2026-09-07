#ifndef DAOBJABUKU_C_H
#define DAOBJABUKU_C_H
#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

/* TWO WITNESSES:
 *
 *   daObjAbuku_c_classInit (historical alias daObjAbuku_c_Spawn)
 *       fBase_c::operator new(276 = 0x114),
 *       dActor_c::dActor_c(), stores the class vtable, then the
 *       dCcAc_c member below.
 *   _ZN12daObjAbuku_cD0Ev  the same member destroyed, then ~dActor_c.
 *
 * SIZE 0x114 is the factory's own literal; mParticle (4 bytes, 0x110) closes
 * exactly on it.
 *
 * RENAMED FROM "Bubble": each overlay's relocs.txt shows this class's own vtable
 * (found via the typeinfo relocation _ZTI12daObjAbuku_c <- vtable+4, the
 * same technique used tree-wide) filling its D1/D0 slots (16/17) with the
 * addresses the tree had previously named _ZN6BubbleD1Ev/_ZN6BubbleD0Ev --
 * an English guess made before the RTTI evidence (_ZTS12daObjAbuku_c, the
 * ROM's own class-name string) existed. Renamed to match the RTTI truth;
 * "Bubble" is also used unrelatedly by the real class LavaBubble.
 *
 * Everything below 0x0d0 duplicated dActor_c's own fields under placeholder
 * names in the old generated header (mPosX/Y/Z at 0x5c/0x60/0x64 and
 * mHorzSpeed at 0x098 already share dActor_c's names). dActor_c ends at
 * exactly 0x0d0, so pad_0d0 (unevidenced, 4 bytes) is this class's own first
 * field. The old header also placed a field at 0x0f8, inside
 * dCcAc_c's own 0x0d4..0x108 span -- daObjAbuku_c_Behavior reads
 * that word directly out of the collision sub-object's own bytes, not out of
 * a field of this class, so no field is declared for it here. The old
 * header also padded 0x10c..0x10e as unevidenced, but
 * daObjAbuku_c_Behavior reads and writes it as a live 16-bit counter, so it
 * is a real field (mSwayAngle) here.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c: only slot 0
 * (InitResources) and slot 6 (Behavior) differ, both still fBase_c's own
 * slots in dActor_c -- CleanupResources, Render, OnPendingDestroy,
 * OnYoshiTryEat and OnTurnIntoEgg are all still the base's own words.
 * The production translation unit defines InitResources and Behavior as real
 * member functions. InitResources is the key function: defining it there emits
 * the class vtable, whose inline destructor slots naturally make mwccarm emit
 * retail's D1-then-D0 pair with no D2 and no forcing scaffold.
 */
struct daObjAbuku_c : dActor_c {
    u8  pad_0d0[0x4];
    /* dCcAc_c member, named by daObjAbuku_c_classInit's own C1 call
       and the class's own destructors' D1 call at +0xd4.
       [src/actors/daObjAbuku_c.cpp] */
    dCcAc_c mdCcAc_c;            /* 0x0d4 */
    /* The same float WingFeather has, one bubble at a time: mSwayAngle
       advances 0x400 a frame, (mSwayAngle >> 4) * 2 + 1 indexes the sin/cos
       table at data_02082214, and that times mDriftSpeed is mHorzSpeed.
       mDriftSpeed eases toward 0x6000. mLifeTimer starts at 0x12c (300 frames)
       and pops the bubble at 0; mParticle is the handle
       Particle::System::New is fed and re-stores.
       [src/actors/daObjAbuku_c.cpp] */
    s32 mDriftSpeed;            /* 0x108 */
    s16 mSwayAngle;            /* 0x10c */
    s16 mLifeTimer;            /* 0x10e */
    s32 mParticle;            /* 0x110 */

    virtual ~daObjAbuku_c() {}            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  Behavior();         /* slot  6 */
};

typedef char daObjAbuku_c_size_must_be_0x114[sizeof(daObjAbuku_c) == 0x114 ? 1 : -1];

#endif
