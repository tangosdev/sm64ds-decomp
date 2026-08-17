#ifndef TILTINGPLATFORMLLL_H
#define TILTINGPLATFORMLLL_H

#include "types.h"

/* Lethal Lava Land's tilting slab.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjGuragura_c, which derives
 * from dBgActor_c, and the difference is in the bytes rather than only in the RTTI:
 * its destructor stores THREE vptrs -- its own, daObjGuragura_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI14daObjFl_Gura_c    ov064 0x0211bce8
 *   _ZTS14daObjFl_Gura_c    ov064 0x0211bcf4
 *   _ZTV18TiltingPlatformLll ov064 0x0211bd2c  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjGuragura_c, ov002 0x0210905c
 *
 * SIZE 0x350, from TiltingPlatformLll_Spawn's literal 848, which daObjGuragura_c
 * fills. NO FIELDS OF ITS OWN. It overrides slots 0 and 3, which the base leaves
 * null.
 *
 * THIS HEADER WAS BUILT FROM THE WRONG FACTORY and that is the reason to read the
 * next paragraph rather than trust the last version. It said `sizeof 0x368`,
 * "MetalNetLift_Spawn asks ActorBase::operator new for 872 bytes", and a `PathPtr
 * mPath` at 0x360 "named by MetalNetLift_Spawn calling _ZN7PathPtrC1Ev at +0x360".
 * MetalNetLift is a different class. Its factory stores _ZTV12MetalNetLift, ov064
 * 0x0211bc68, and never mentions this one; this class's factory is
 * TiltingPlatformLll_Spawn, which allocates 848 = 0x350, stores
 * _ZTV15daObjGuragura_c and then _ZTV18TiltingPlatformLll, ov064 0x0211bd2c, and
 * constructs no PathPtr. The two vtables are 0xc4 apart in the same overlay, which
 * is presumably how they were crossed. Both relocation sets are in
 * config/arm9/overlays/ov064/relocs.txt and they do not overlap.
 *
 * Nothing consumed the wrong fields: this class's two methods are cross-overlay
 * veneers that pass `this` and a table pointer and touch no member at all.
 */

#ifdef __cplusplus

#include "daObjGuragura_c.h"

struct TiltingPlatformLll : daObjGuragura_c {
    /* --- vtable --- */
    virtual ~TiltingPlatformLll();     /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char TiltingPlatformLll_size_must_be_0x350[sizeof(TiltingPlatformLll) == 0x350 ? 1 : -1];

#endif /* __cplusplus */

#endif /* TILTINGPLATFORMLLL_H */
