#ifndef DAOBJKURUMAJIKU_C_H
#define DAOBJKURUMAJIKU_C_H

#include "types.h"
#include "Platform.h"

/* The rickshaw axle: an abstract Platform that carries four mounted actors and
 * spins them around itself. `kurumajiku` is the axle of a wheeled cart.
 *
 * A LAYER THE TREE DID NOT HAVE. The ROM's RTTI names this class and points it
 * at its base; the tree named the base's base instead, so both descendants read
 * as direct Platform subclasses.
 *
 *   _ZTI17daObjKurumajiku_c  ov002 0x02109320-4 -> the record at 0x021092f8
 *   _ZTS17daObjKurumajiku_c  ov002 0x02109304   "17daObjKurumajiku_c"
 *   vtable                   ov002 0x02109320, 32 slots, same count as the base
 *   kind                     __si_class_type_info, ONE base, subobject offset 0
 *   base                     dBgActor_c, ov002 0x021089ec -- the tree's Platform
 *
 * ABSTRACT. Slots 0 and 3 -- InitResources and CleanupResources -- are null.
 * Its own overrides are slots 6 (Behavior), 9 (Render), 16 (D1) and 17 (D0);
 * every other slot holds the base's word and is inherited.
 *
 * TWO DESCENDANTS, and the ROM's own destructors say so independently of the
 * RTTI: _ZN11RickshawBdwD1Ev stores this class's vtable between its own and
 * _ZTV8Platform, and so does the ov047 sibling daObjKm3_Kurumajiku_c. A class
 * that derived from Platform directly would store two vptrs, not three.
 *
 * THE FOUR IDs AT 0x320 ARE THIS CLASS'S, not a descendant's. They are read by
 * `func_ov002_020b6b38`, which is THIS CLASS'S OWN Behavior (vtable slot 6, and
 * it lives in ov002 with the class, not in either child's overlay). A method can
 * reach its own members and its ancestors', never a descendant's. Behavior walks
 * the four, looks each up with Actor::FindWithID and writes the actor's position
 * from a rotation of this platform's own angles -- four things bolted to the
 * axle. The name is coined from what the code does; nothing in the ROM names it.
 *
 * SIZE 0x330 closes on that: Platform ends at 0x31e, the array is 4-aligned so it
 * starts at 0x320, and 0x320 + 4*4 = 0x330 is the literal both factories pass to
 * ActorBase::operator new -- RickshawBdw_Spawn and RickshawBs_Spawn, one per
 * descendant, agreeing.
 */

#ifdef __cplusplus

struct daObjKurumajiku_c : Platform {
    /* Field NAME is a placeholder; the offset and the width are observed. */
    u32 mMountedActorIds[4];        /* 0x320 */

    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/Platform.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN17daObjKurumajiku_cD1Ev (which does exist out of line, at ov002
       0x020b6a3c, still under its func_ov002_ name). An out-of-line declaration
       here would make each descendant emit a `bl` the ROM does not have. */
    virtual ~daObjKurumajiku_c() {}
};

typedef char daObjKurumajiku_c_size_must_be_0x330[sizeof(daObjKurumajiku_c) == 0x330 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKURUMAJIKU_C_H */
