#ifndef DAOBJKURUMA_C_H
#define DAOBJKURUMA_C_H

#include "types.h"
#include "Platform.h"

/* The rickshaw's own platform: an abstract Platform that adds behaviour and no
 * state. `kuruma` is the cart.
 *
 * A LAYER THE TREE DID NOT HAVE. The ROM's RTTI names this class and points it
 * at its base; the tree named the base's base instead.
 *
 *   _ZTI13daObjKuruma_c  ov002 0x02109254
 *   _ZTS13daObjKuruma_c  ov002 0x02109260   "13daObjKuruma_c"
 *   vtable               ov002 0x02109278, 32 slots, same count as the base
 *   kind                 __si_class_type_info, ONE base, subobject offset 0
 *   base                 dBgActor_c, ov002 0x021089ec -- the tree's Platform
 *
 * ABSTRACT. Slots 0 and 3 -- InitResources and CleanupResources -- are null.
 * Its own overrides are slots 6 (Behavior), 9 (Render), 16 (D1) and 17 (D0).
 *
 * TWO DESCENDANTS: RickshawPlatformBdw (daObjKm1_Kuruma_c) and the unnamed ov047
 * sibling daObjKm3_Kuruma_c. Both destructors store this class's vtable between
 * their own and _ZTV8Platform, which is the same fact the RTTI records.
 *
 * NO FIELDS OF ITS OWN, and that is measured rather than assumed. Its own
 * Behavior (`func_ov002_020b6920`) and Render (`func_ov002_020b68f8`) reach
 * nothing above Platform -- Behavior is UpdateModelPosAndRotY plus the guarded
 * UpdateClsnPosAndRot, Render dispatches through the Model at 0xd4. Its own
 * destructor destroys only Platform's two members. And both factories,
 * RickshawPlatformBdw_Spawn and RickshawPlatformBs_Spawn, pass 800 = 0x320 to
 * ActorBase::operator new, which is sizeof(Platform) exactly: there is no room
 * for a field anywhere in this class or in either leaf.
 */

#ifdef __cplusplus

struct daObjKuruma_c : Platform {
    /* --- vtable --- */
    /* INLINE ON PURPOSE -- see include/Platform.h. The out-of-line copy is at
       ov002 0x020b686c, still under its func_ov002_ name. */
    virtual ~daObjKuruma_c() {}
};

typedef char daObjKuruma_c_size_must_be_0x320[sizeof(daObjKuruma_c) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKURUMA_C_H */
