#ifndef DAOBJHMMARUTA_C_H
#define DAOBJHMMARUTA_C_H

#include "types.h"

/* Tall, Tall Mountain's rolling log. `hm` is the ROM's tag for that stage.
 *
 * NAME. The cartridge spells this class daObjHmMaruta_c. Read straight out of
 * extracted/overlays/overlay_0030.bin (ov030 loads at 0x021111a0):
 *
 *   _ZTV15daObjHmMaruta_c  ov030 0x02115a48  address point; storage starts at
 *                          0x02115a40 = [0x00000000, 0x021159f8]
 *   _ZTI15daObjHmMaruta_c  ov030 0x021159f8  = [0x0209a764 __si_class_type_info,
 *                          0x02115a10, 0x02114350 _ZTI13daObjMaruta_c (ov022)]
 *   _ZTS15daObjHmMaruta_c  ov030 0x02115a10  = "15daObjHmMaruta_c\0"
 *
 * So the vtable's typeinfo word names the same triple, and the tree's former
 * spelling, RollingLogTtm, was coined: it lived on the vtable and the five
 * members only, while the _ZTI/_ZTS records already carried this name.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjMaruta_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjMaruta_c's, then dBgActor_c's.
 * A one-level chain emits two. kind __si_class_type_info, ONE base, subobject
 * offset 0.
 *
 * SIZE 0x344, from daObjHmMaruta_c_classInit's literal 836 (historical alias
 * RollingLogTtm_Spawn). It overrides slots 0, 3 and 6,
 * all three of which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjMaruta_c.h"

struct daObjHmMaruta_c : daObjMaruta_c {
    /* --- vtable --- */
    /* INLINE AND EMPTY, ON PURPOSE. Written out of line, this destructor is the
       key function and mwccarm lowers it as D0 ahead of D1 plus a D2 the
       cartridge has no home for; defined here, the key function moves to
       Behavior and the vtable's slots 16 and 17 emit D1 then D0, which is the
       ROM order (D1 0x0211155c, D0 0x021115ac). The three vptr stores are
       daObjMaruta_c's inline destructor and dBgActor_c's, not a body here. */
    virtual ~daObjHmMaruta_c() {}    /* slots 16 (D1), 17 (D0) */

    int Behavior();                    /* slot  6 */
    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjHmMaruta_c_size_must_be_0x344[sizeof(daObjHmMaruta_c) == 0x344 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJHMMARUTA_C_H */
