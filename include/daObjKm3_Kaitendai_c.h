#ifndef DAOBJKM3_KAITENDAI_C_H
#define DAOBJKM3_KAITENDAI_C_H

#include "types.h"

/* Bowser in the Sky's spinning disc.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjKaitendai_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjKaitendai_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI20daObjKm3_Kaitendai_c  ov047 0x02112328
 *   _ZTS20daObjKm3_Kaitendai_c  ov047 0x02112340
 *   _ZTV20daObjKm3_Kaitendai_c  ov047 0x0211237c  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjKaitendai_c, ov002 0x021091ac
 *
 * NO FIELDS OF ITS OWN, and the factory that says so is NOT the one named after this
 * class. `func_ov047_021113bc`, still unnamed, is the function that allocates 800 =
 * 0x320 and stores this class's vtable second. daObjKm3_Kurumajiku_c_classInit
 * (historical alias RickshawBs_Spawn) allocates 816 and
 * builds daObjKm3_Kurumajiku_c: the ov047 "Bs" names were crossed -- this class was
 * misnamed RickshawBs (a #1521-era mixup) until the rename below, resolved by vtable
 * evidence rather than by either factory's own name. The Kurumajiku factory is
 * paired independently from the C++ class it builds (see
 * include/daObjKm3_Kurumajiku_c.h). It overrides slots 0
 * and 3, which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjKaitendai_c.h"

struct daObjKm3_Kaitendai_c : daObjKaitendai_c {
    /* --- vtable --- */
    /* The inline empty body is codegen-significant: mwccarm emits the retail
     * D1/D0 pair in that order without materializing a homeless D2 body. */
    virtual ~daObjKm3_Kaitendai_c() {}  /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char daObjKm3_Kaitendai_c_size_must_be_0x320[sizeof(daObjKm3_Kaitendai_c) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKM3_KAITENDAI_C_H */
