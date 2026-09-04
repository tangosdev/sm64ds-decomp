#ifndef VOLCANORING_H
#define VOLCANORING_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjFl_Ring_c_classInit  fBase_c::operator new(808 = 0x328), dBgActor_c::dBgActor_c(), stores _ZTV11VolcanoRing,
 *                 then the members below in this order.
 *   ~VolcanoRing   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x328 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct VolcanoRing : dBgActor_c {
    /* dBgActor_c ends at 0x31e (its own sizeof rounds 0x31e up to 0x320);
       these two bytes are this class's own, in the base's tail padding --
       same shape as daObjRc_Guruguru_c's single s16 there (see
       include/dBgActor_c.h). Field names are placeholders: offsets, widths
       and read/write sites are observed from Behavior (slot 6, ov022
       0x021112ac), not proven semantics. */
    u8  mState;            /* 0x31e -- Behavior's own two-case switch */
    u8  mSpawnGuard;        /* 0x31f -- DecIfAbove0_Byte-gated once per spawn */
    u8  mTriggered;        /* 0x320 -- read once, reset to 0 every Behavior call */
    u8  pad_321[0x1];
    u16 mCooldown;          /* 0x322 -- DecIfAbove0_Short, gates the whole switch */
    u16 mSpawnCount;        /* 0x324 -- incremented once per actorID 0xf3 spawn */
    u8  pad_326[0x2];

    virtual ~VolcanoRing();            /* slots 16 (D1), 17 (D0) */

    int InitResources();     /* slot 0 */
    int CleanupResources();  /* slot 3 */
    int Behavior();          /* slot 6 */
    int Render();            /* slot 9 */
};

typedef char VolcanoRing_size_must_be_0x328[sizeof(VolcanoRing) == 0x328 ? 1 : -1];

#endif /* VOLCANORING_H */
