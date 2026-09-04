#ifndef DAOBJBSWDOOR_C_H
#define DAOBJBSWDOOR_C_H

#include "types.h"

/* Bob-omb Battlefield's switch-operated shutter.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjSwdoor_c, which derives
 * from dBgActor_c, and the difference is in the bytes rather than only in the RTTI:
 * its destructor stores THREE vptrs -- its own, daObjSwdoor_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI14daObjBSwdoor_c  ov014 0x021145b8
 *   _ZTS14daObjBSwdoor_c  ov014 0x021145d0
 *   _ZTV14daObjBSwdoor_c      ov014 0x02114608  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjSwdoor_c, ov002 0x021099c0
 *
 * SIZE 0x324, from daObjBSwdoor_c_classInit's literal 804. It overrides slots 0
 * (InitResources), 3 (CleanupResources) and 6 (Behavior), all three of which the
 * base leaves null.
 *
 * THE FOUR BYTES AT 0x320 ARE UNOBSERVED. Nothing in the tree reads them, on this
 * class or on daObjSwdoor_c; they are declared here rather than on the base
 * because the base's Behavior slot is null, so no method of the base could be
 * reading them. See include/daObjSwdoor_c.h.
 *
 * WHAT THIS HEADER USED TO SAY: a `u8 mModel` marker at 0xd4 and a
 * `u8 mMovingMeshCollider` marker at 0x124, over `u8 pad_000[0xd4]`. Both were
 * dBgActor_c's, twice removed, and are inherited now -- dBgActor_c calls the second
 * one mMeshCollider, which is why _ZN14daObjBSwdoor_c13InitResourcesEv.cpp changes
 * with this header.
 *
 * SM64DS RTTI names the implementation daObjBSwdoor_c. The reconstructed factory
 * daObjBSwdoor_c_classInit (historical alias daObjBSwdoor_c_Spawn) installs this class's
 * cartridge vtable for the SWITCHDOOR registry profile.
 */

#ifdef __cplusplus

#include "daObjSwdoor_c.h"

struct daObjBSwdoor_c : daObjSwdoor_c {
    u8  pad_320[0x4];

    /* --- vtable --- */
    /* INLINE, AND THAT IS WHAT LETS THIS CLASS OWN A TRANSLATION UNIT. Declared
       out of line, mwccarm 2004/b56 emits D0 before D1 -- the reverse of the
       cartridge, which has D1 at 0x021111a0 and D0 at 0x021111f0 -- and adds a
       homeless D2 that no address claims; tools/objisolate.py then refuses the
       whole TU rather than one function. Defined here, the pair comes out in ROM
       order with no D2.

       SAFE ONLY BECAUSE THIS CLASS IS A LEAF, and that is measured rather than
       assumed: _ZTI14daObjBSwdoor_c, ov014 0x021145b8, occurs exactly once as a
       word anywhere under extracted/ -- at ov014 file offset 0x3464, which is
       0x02114604, the typeinfo slot of its own vtable header. No other class's
       __si_class_type_info points at it, so no descendant exists to inline this
       body where the ROM has a `bl`.

       THREE vtable stores come out of it. `daObjBSwdoor_c : daObjSwdoor_c :
       dBgActor_c` emits its own vptr, then daObjSwdoor_c's -- inlined, that
       destructor is defined in its class body too -- then dBgActor_c's, then
       dBgActor_c's dBgW_KcMbg and Model, then dActor_c. Nothing in the chain adds
       a member with a destructor, so the body is empty. */
    virtual ~daObjBSwdoor_c() {}           /* slots 16 (D1), 17 (D0) */

    int Behavior();                    /* slot  6 */
    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char daObjBSwdoor_c_size_must_be_0x324[sizeof(daObjBSwdoor_c) == 0x324 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJBSWDOOR_C_H */
