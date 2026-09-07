#ifndef DAOBJFL_GURA_C_H
#define DAOBJFL_GURA_C_H

#include "types.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Lethal Lava Land's tilting slab.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjGuragura_c, which derives
 * from dBgActor_c, and the difference is in the bytes rather than only in the RTTI:
 * its destructor stores THREE vptrs -- its own, daObjGuragura_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI14daObjFl_Gura_c    ov064 0x0211bce8
 *   _ZTS14daObjFl_Gura_c    ov064 0x0211bcf4
 *   _ZTV14daObjFl_Gura_c ov064 0x0211bd2c  (public address point)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjGuragura_c, ov002 0x0210905c
 *
 * SIZE 0x350, from daObjFl_Gura_c_classInit's literal 848, which daObjGuragura_c
 * fills. NO FIELDS OF ITS OWN. It overrides slots 0 and 3, which the base leaves
 * null.
 *
 * THIS HEADER WAS BUILT FROM THE WRONG FACTORY and that is the reason to read the
 * next paragraph rather than trust the last version. It said `sizeof 0x368`,
 * "daObjFl_Amilift_c_classInit asks fBase_c::operator new for 872 bytes", and a
 * `PathPtr mPath` at 0x360 "named by daObjFl_Amilift_c_classInit calling
 * _ZN7PathPtrC1Ev at +0x360".
 * MetalNetLift is a different class. Its factory stores _ZTV12MetalNetLift, ov064
 * 0x0211bc68, and never mentions this one; this class's factory is
 * daObjFl_Gura_c_classInit, which allocates 848 = 0x350, stores
 * _ZTV15daObjGuragura_c and then _ZTV14daObjFl_Gura_c, ov064 0x0211bd2c, and
 * constructs no PathPtr. The two vtables are 0xc4 apart in the same overlay, which
 * is presumably how they were crossed. Both relocation sets are in
 * config/arm9/overlays/ov064/relocs.txt and they do not overlap.
 *
 * Nothing consumed the wrong fields: this class's two methods are cross-overlay
 * veneers that pass `this` and a table pointer and touch no member at all.
 */

#ifdef __cplusplus

#include "daObjGuragura_c.h"

struct daObjFl_Gura_c : daObjGuragura_c {
    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */

    static void *operator new(unsigned long size);

    /* Declared last and inline so class instantiation emits the retail D1/D0
       pair in cartridge order without a separate leaf D2 body. */
    virtual ~daObjFl_Gura_c() {}
};

inline void *daObjFl_Gura_c::operator new(unsigned long size)
{
    return _ZN7fBase_cnwEj((unsigned)size);
}

typedef char daObjFl_Gura_c_size_must_be_0x350[sizeof(daObjFl_Gura_c) == 0x350 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJFL_GURA_C_H */
