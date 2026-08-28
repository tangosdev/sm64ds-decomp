#ifndef DAOBJKM1_DORIFU_C_H
#define DAOBJKM1_DORIFU_C_H

#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

/* The Bob-omb Battlefield drifting stairs. The tree's factory for it is
 * StairsBdw_Spawn; the class itself is only ever named by its ROM name.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjDorifu_c, which derives
 * from dBgActor_c. The destructor stores three vptrs -- its own, daObjDorifu_c's,
 * then dBgActor_c's -- and destroys daObjDorifu_c's Model[5] and
 * dBgW_KcMbg[5] in between. All of that comes out of the base
 * declaration now, so the destructor body is empty.
 *
 *   _ZTI17daObjKm1_Dorifu_c  ov043 0x021124dc
 *   _ZTS17daObjKm1_Dorifu_c  ov043 0x021124e8
 *   _ZTV17daObjKm1_Dorifu_c  ov043 0x0211255c  (its record sits at V-4)
 *   kind                     __si_class_type_info, ONE base, offset 0
 *   base                     daObjDorifu_c, ov002 0x02108d70
 *
 * NO FIELDS OF ITS OWN: StairsBdw_Spawn passes 0xdcc, which daObjDorifu_c fills.
 * It overrides slots 0 and 3, which the base leaves null. Everything this header
 * used to restate -- a `u8 mModel` marker at 0xd4 and a `u8 mMovingMeshCollider`
 * marker at 0x124 -- was dBgActor_c's, twice removed, and is inherited now.
 */

#ifdef __cplusplus

#include "daObjDorifu_c.h"

struct daObjKm1_Dorifu_c : daObjDorifu_c {
    /* --- vtable --- */
    virtual ~daObjKm1_Dorifu_c() {}      /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char daObjKm1_Dorifu_c_size_must_be_0xdcc[sizeof(daObjKm1_Dorifu_c) == 0xdcc ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/dBgActor_c.h. */
struct daObjKm1_Dorifu_c {
    u8  pad_000[0xd4];
    /* Model member. The cartridge's own ~daObjKm1_Dorifu_c calls _ZN5ModelD1Ev at
       +0x0d4 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~daObjKm1_Dorifu_c calls
       _ZN10dBgW_KcMbgD1Ev at +0x124 (D0/D1), a relocation the ROM build checks;
       recovered by tools/dtor_members.py. D1 and not D2, so it is this type and not an
       inlined base. */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
};

#endif /* __cplusplus */

#endif /* DAOBJKM1_DORIFU_C_H */
