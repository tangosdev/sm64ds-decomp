#ifndef DAOBJKM3_KURUMAJIKU_C_H
#define DAOBJKM3_KURUMAJIKU_C_H

#include "types.h"
#include "daObjKurumajiku_c.h"

/* Bowser in the Sky's rickshaw axle. Its factory is named RickshawBs_Spawn --
 * "Bs" for Bowser in the Sky -- but that name is crossed with its sibling's
 * (include/daObjKm3_Kaitendai_c.h documents the mixup: the ov047 "Bs"-named
 * C++ class actually reproduces daObjKm3_Kaitendai_c, not this one -- it was
 * misnamed RickshawBs until that header's rename). This class stays unnamed
 * in the tree until now; paired to its vtable by ADDRESS, not by the
 * factory's name.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjKurumajiku_c, which
 * derives from dBgActor_c, and the difference is in the bytes rather than only in
 * the RTTI: its destructor stores THREE vptrs -- its own, daObjKurumajiku_c's,
 * then dBgActor_c's (func_ov047_021111a0 / func_ov047_021111f0, both `complete`
 * today under placeholder names). A one-level chain emits two.
 *
 *   _ZTI21daObjKm3_Kurumajiku_c  ov047 0x0211224c
 *   _ZTS21daObjKm3_Kurumajiku_c  ov047 0x02112264
 *   _ZTV21daObjKm3_Kurumajiku_c  ov047 0x021122a0  (storage starts at 0x02112298)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjKurumajiku_c, ov002 0x021092f8
 *
 * NO FIELDS OF ITS OWN: RickshawBs_Spawn passes 816 = 0x330, which
 * daObjKurumajiku_c already fills exactly. It overrides slots 0 and 3, which
 * the base leaves null.
 */

#ifdef __cplusplus

struct daObjKm3_Kurumajiku_c : daObjKurumajiku_c {
    /* --- vtable --- */
    /* Defining this empty destructor in the class body is codegen-significant:
     * mwccarm emits the used D1/D0 pair in retail order and does not materialize
     * the otherwise homeless D2 body. The guard lets the shadow TU prove that
     * form while the legacy D1/D0 intake files remain enrolled; promotion removes
     * those files and makes the inline form unconditional. */
#ifdef DAOBJKM3_KURUMAJIKU_INLINE_DTOR
    virtual ~daObjKm3_Kurumajiku_c() {}    /* slots 16 (D1), 17 (D0) */
#else
    virtual ~daObjKm3_Kurumajiku_c();      /* slots 16 (D1), 17 (D0) */
#endif

    int CleanupResources();                /* slot  3 */
    int InitResources();                   /* slot  0 */
};

typedef char daObjKm3_Kurumajiku_c_size_must_be_0x330[sizeof(daObjKm3_Kurumajiku_c) == 0x330 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKM3_KURUMAJIKU_C_H */
