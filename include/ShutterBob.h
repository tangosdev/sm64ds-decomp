#ifndef SHUTTERBOB_H
#define SHUTTERBOB_H

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
 *   _ZTV10ShutterBob      ov014 0x02114608  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjSwdoor_c, ov002 0x021099c0
 *
 * SIZE 0x324, from ShutterBob_Spawn's literal 804. It overrides slots 0
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
 * one mMeshCollider, which is why _ZN10ShutterBob13InitResourcesEv.cpp changes
 * with this header.
 */

#ifdef __cplusplus

#include "daObjSwdoor_c.h"

struct ShutterBob : daObjSwdoor_c {
    u8  pad_320[0x4];

    /* --- vtable --- */
    virtual ~ShutterBob() {}             /* slots 16 (D1), 17 (D0) */

    int Behavior();                    /* slot  6 */
    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char ShutterBob_size_must_be_0x324[sizeof(ShutterBob) == 0x324 ? 1 : -1];

#endif /* __cplusplus */

#endif /* SHUTTERBOB_H */
