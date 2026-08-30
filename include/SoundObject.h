#ifndef SOUNDOBJECT_H
#define SOUNDOBJECT_H

#include "types.h"
#include "dActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   SoundObject_Spawn  fBase_c::operator new(228 = 0xe4), dActor_c::dActor_c(), stores _ZTV11SoundObject,
 *                 then the members below in this order.
 *   ~SoundObject   the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0xe4 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct SoundObject : dActor_c {
    u8  pad_0d0[0x4];
    s32 mLevelID;                /* 0x0d4 */
    s32 mTimerThreshold;         /* 0x0d8 */
    u16 mTimer;                  /* 0x0dc */
    u16 mTimerReset;             /* 0x0de */
    u8  unk_0e0;                 /* 0x0e0 */
    u8  pad_0e1[0x3];

    virtual ~SoundObject() {}            /* slots 16 (D1), 17 (D0) */

    int Behavior();
    int InitResources();
};

typedef char SoundObject_size_must_be_0xe4[sizeof(SoundObject) == 0xe4 ? 1 : -1];

#endif /* SOUNDOBJECT_H */
