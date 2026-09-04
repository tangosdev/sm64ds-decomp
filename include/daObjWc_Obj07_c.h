#ifndef DAOBJWC_OBJ07_C_H
#define DAOBJWC_OBJ07_C_H

#include "types.h"

/* Wet-Dry World's spinning disc.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjKaitendai_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjKaitendai_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI15daObjWc_Obj07_c  ov029 0x02113fc8
 *   _ZTS15daObjWc_Obj07_c  ov029 0x02113fe0
 *   _ZTV15daObjWc_Obj07_c  ov029 0x02114018  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjKaitendai_c, ov002 0x021091ac
 *
 * NO FIELDS OF ITS OWN: daObjWc_Obj07_c_classInit passes 800 = 0x320 = sizeof(dBgActor_c).
 * It overrides slots 0 and 3, which the base leaves null.
 *
 * SM64DS RTTI names the implementation daObjWc_Obj07_c. The reconstructed factory
 * daObjWc_Obj07_c_classInit (historical alias daObjWc_Obj07_c_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_WC_UKISIMA
 * (historical alias daObjWc_Obj07_c_SpawnInfo) is its registry descriptor.
 */

#ifdef __cplusplus

#include "daObjKaitendai_c.h"

struct daObjWc_Obj07_c : daObjKaitendai_c {
    /* --- vtable. The destructor is INLINE, and that is load-bearing rather
       than a style choice. Out of line, mwccarm emits D0 before D1 -- the
       reverse of the ROM's 0x02112080 D1 / 0x021120d0 D0 order, which makes
       objisolate refuse the whole TU -- and additionally emits a D2 that has
       no home in the cartridge. Inline, it emits exactly the retail D1/D0 pair
       in ROM order and no D2. Nothing derives from this class, so no
       descendant needs a D2 to `bl`: its _ZTI at ov029 0x02113fc8 appears as a
       base pointer in no other RTTI record anywhere in the image (the one
       other 32-bit occurrence of that value is in ov014, which loads at the
       SAME address 0x021111a0 as ov029 and is therefore never resident
       alongside it -- a coincidence, not a derivation). --- */
    virtual ~daObjWc_Obj07_c() {}

    /* Declared before InitResources deliberately: with the destructor inline
       the key function is the first DECLARED non-inline virtual -- declared,
       not lowest slot -- so this ordering is what makes
       src/actors/d_a_obj_wc_obj07.cpp the TU that emits the _ZTV/_ZTI/_ZTS
       group. Both are real methods for the same reason: a hand-mangled
       `extern "C"` free function does not DEFINE the key function, and then
       mwcc emits neither the RTTI group nor the inline destructor's D1/D0
       pair. `virtual` is written out for the reader; these override
       daObjKaitendai_c slots and would be virtual either way. */
    virtual s32 CleanupResources();    /* slot  3 */
    virtual s32 InitResources();       /* slot  0 */
};

typedef char daObjWc_Obj07_c_size_must_be_0x320[sizeof(daObjWc_Obj07_c) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJWC_OBJ07_C_H */
