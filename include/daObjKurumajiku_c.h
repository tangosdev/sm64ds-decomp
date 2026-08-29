#ifndef DAOBJKURUMAJIKU_C_H
#define DAOBJKURUMAJIKU_C_H

#include "types.h"
#include "dBgActor_c.h"

/* The rickshaw axle: an abstract dBgActor_c that carries four mounted actors and
 * spins them around itself. `kurumajiku` is the axle of a wheeled cart.
 *
 * A LAYER THE TREE DID NOT HAVE. The ROM's RTTI names this class and points it
 * at its base; the tree named the base's base instead, so both descendants read
 * as direct dBgActor_c subclasses.
 *
 *   _ZTI17daObjKurumajiku_c  ov002 0x02109320-4 -> the record at 0x021092f8
 *   _ZTS17daObjKurumajiku_c  ov002 0x02109304   "17daObjKurumajiku_c"
 *   vtable                   ov002 0x02109320, 32 slots, same count as the base
 *   kind                     __si_class_type_info, ONE base, subobject offset 0
 *   base                     dBgActor_c, ov002 0x021089ec -- the tree's dBgActor_c
 *
 * ABSTRACT. Slots 0 and 3 -- InitResources and CleanupResources -- are null.
 * Its own overrides are slots 6 (Behavior), 9 (Render), 16 (D1) and 17 (D0);
 * every other slot holds the base's word and is inherited.
 *
 * TWO DESCENDANTS, and the ROM's own destructors say so independently of the
 * RTTI: _ZN11RickshawBdwD1Ev stores this class's vtable between its own and
 * _ZTV10dBgActor_c, and so does the ov047 sibling daObjKm3_Kurumajiku_c. A class
 * that derived from dBgActor_c directly would store two vptrs, not three.
 *
 * THE FOUR IDs AT 0x320 ARE THIS CLASS'S, not a descendant's. They are read by
 * `func_ov002_020b6b38`, which is THIS CLASS'S OWN Behavior (vtable slot 6, and
 * it lives in ov002 with the class, not in either child's overlay). A method can
 * reach its own members and its ancestors', never a descendant's. Behavior walks
 * the four, looks each up with dActor_c::FindWithID and writes the actor's position
 * from a rotation of this platform's own angles -- four things bolted to the
 * axle. The name is coined from what the code does; nothing in the ROM names it.
 *
 * SIZE 0x330 closes on that: dBgActor_c ends at 0x31e, the array is 4-aligned so it
 * starts at 0x320, and 0x320 + 4*4 = 0x330 is the literal both factories pass to
 * fBase_c::operator new -- RickshawBdw_Spawn and RickshawBs_Spawn, one per
 * descendant, agreeing.
 */

#ifdef __cplusplus

struct daObjKurumajiku_c : dBgActor_c {
    /* Field NAME is a placeholder; the offset and the width are observed. */
    u32 mMountedActorIds[4];        /* 0x320 */

    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/dBgActor_c.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN17daObjKurumajiku_cD1Ev (which does exist out of line, at ov002
       0x020b6a3c, still under its func_ov002_ name). An out-of-line declaration
       here would make each descendant emit a `bl` the ROM does not have. */
    virtual ~daObjKurumajiku_c() {}

    /* Slot 6, ov002 0x020b6b38 -- carries the four mounted actors around the
       axle. An override of the virtual fBase_c already declared, so it
       occupies a slot that exists rather than adding one, and no field moves.
       Spelled without `virtual` the way include/daObjFloatBoard_c.h spells its
       own slot-3 override: virtualness is inherited, and the destructor above
       stays the first virtual DECLARED in the class. */
    s32 Behavior();
    s32 Render();                      /* slot 9, ov002 0x020b6b10 -- mModel.Render(0) */
    /* THE NULL SLOTS THE NOTE ABOVE ALREADY NAMES, SPELT SO THE COMPILER AGREES.
       mwccarm lays down a bare 0x00000000 with no relocation for a pure virtual --
       there is no __cxa_pure_virtual in this image for it to point at -- so a zero
       word in a ROM vtable IS the `= 0`, and it is the only thing that produces one.
       Left undeclared, this class silently inherits dBgActor_c's concrete bodies and
       the vtable it emits disagrees with the cartridge at exactly these slots.
       Measured by tools/romdata_check.py, which is the only gate that reads them:
       the ROM build's 106/106 compares .text alone and is blind here.

       DECLARED LAST, AND WITH `virtual` -- unlike the plain overrides above. The
       pure-specifier is only valid on a declaration carrying the keyword, and a
       pure virtual has no body to emit, so it can never become the key function:
       whichever virtual was first and non-inline before is still first now. */
    virtual s32 InitResources() = 0;        /* slot  0 */
    virtual s32 CleanupResources() = 0;     /* slot  3 */
};

typedef char daObjKurumajiku_c_size_must_be_0x330[sizeof(daObjKurumajiku_c) == 0x330 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKURUMAJIKU_C_H */
