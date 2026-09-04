#ifndef POPPINGLAVABUBBLES_H
#define POPPINGLAVABUBBLES_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daObjLava_c -- daObjLava_c_classInit (was PoppingLavaBubbles_Spawn), g_profile_LAVA (was PoppingLavaBubbles_SpawnInfo)
 */

#include "types.h"
#include "dActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjLava_c_classInit  fBase_c::operator new(216 = 0xd8), dActor_c::dActor_c(), stores _ZTV18PoppingLavaBubbles,
 *                 then the members below in this order.
 *   ~PoppingLavaBubbles   the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0xd8 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct PoppingLavaBubbles : dActor_c {
    u8  pad_0d0[0x8];

    virtual ~PoppingLavaBubbles();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
};

typedef char PoppingLavaBubbles_size_must_be_0xd8[sizeof(PoppingLavaBubbles) == 0xd8 ? 1 : -1];

#endif /* POPPINGLAVABUBBLES_H */
