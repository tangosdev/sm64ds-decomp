#ifndef DAOBJKM1_DORIFU_C_H
#define DAOBJKM1_DORIFU_C_H

#include "types.h"

/* The Bob-omb Battlefield drifting stairs. Its factory is
 * daObjKm1_Dorifu_c_classInit; the class itself is named by its ROM RTTI identity.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjDorifu_c, which derives
 * from dBgActor_c. The destructor stores three vptrs -- its own, daObjDorifu_c's,
 * then dBgActor_c's -- and destroys daObjDorifu_c's Model[5] and
 * dBgW_KcMbg[5] in between. All of that comes out of the base
 * declaration now, so the destructor body is empty.
 *
 *   _ZTI17daObjKm1_Dorifu_c  ov043 0x021124dc
 *   _ZTS17daObjKm1_Dorifu_c  ov043 0x021124e8
 *   _ZTV17daObjKm1_Dorifu_c  ov043 0x0211255c  (storage starts at 0x02112554)
 *   kind                     __si_class_type_info, ONE base, offset 0
 *   base                     daObjDorifu_c, ov002 0x02108d70
 *
 * NO FIELDS OF ITS OWN: daObjKm1_Dorifu_c_classInit passes 0xdcc, which
 * daObjDorifu_c fills. It overrides slots 0 and 3, which the base leaves null.
 * Everything this header
 * used to restate -- a `u8 mModel` marker at 0xd4 and a `u8 mMovingMeshCollider`
 * marker at 0x124 -- was dBgActor_c's, twice removed, and is inherited now.
 */

#ifdef __cplusplus

#include "daObjDorifu_c.h"

struct daObjKm1_Dorifu_c : daObjDorifu_c {
    /* --- vtable --- */
    /* The inline empty body is codegen-significant: mwccarm emits the retail
     * D1/D0 pair in that order without materializing a homeless D2 body. */
    virtual ~daObjKm1_Dorifu_c() {}    /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char daObjKm1_Dorifu_c_size_must_be_0xdcc[sizeof(daObjKm1_Dorifu_c) == 0xdcc ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKM1_DORIFU_C_H */
