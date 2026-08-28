#ifndef DAOBJRC_DORIFU_C_H
#define DAOBJRC_DORIFU_C_H

#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

/* The Rainbow Ride drifting platform. Only ever named by its ROM name.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjDorifu_c, which derives
 * from dBgActor_c. The destructor stores three vptrs -- its own, daObjDorifu_c's,
 * then dBgActor_c's -- and destroys daObjDorifu_c's Model[5] and
 * dBgW_KcMbg[5] in between, all of it from the base declaration.
 *
 *   _ZTI16daObjRc_Dorifu_c  ov036 0x02113e4c
 *   _ZTS16daObjRc_Dorifu_c  ov036 0x02113e58
 *   _ZTV16daObjRc_Dorifu_c  ov036 0x02113ecc  (its record sits at V-4)
 *   kind                    __si_class_type_info, ONE base, offset 0
 *   base                    daObjDorifu_c, ov002 0x02108d70 -- CROSSING OVERLAYS,
 *                           which is ordinary here: 183 of the ROM's 413 base
 *                           edges leave their overlay.
 *
 * NO FIELDS OF ITS OWN: its factory, TrickyTriangles_Spawn, passes 0xdcc to
 * fBase_c::operator new, which daObjDorifu_c fills. It overrides slots 0 and 3,
 * which the base leaves null, and both bodies just forward to daObjDorifu_c's
 * shared helpers in ov002 with this class's own argument block. Everything this
 * header used to restate was dBgActor_c's, twice removed.
 */

#ifdef __cplusplus

#include "daObjDorifu_c.h"

struct daObjRc_Dorifu_c : daObjDorifu_c {
    /* --- vtable --- */
    virtual ~daObjRc_Dorifu_c() {}       /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char daObjRc_Dorifu_c_size_must_be_0xdcc[sizeof(daObjRc_Dorifu_c) == 0xdcc ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/dBgActor_c.h. */
struct daObjRc_Dorifu_c {
    u8  pad_000[0xd4];
    /* Model member. The cartridge's own ~daObjRc_Dorifu_c calls _ZN5ModelD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~daObjRc_Dorifu_c calls
       _ZN10dBgW_KcMbgD1Ev at +0x124 (D0/D1), a relocation the ROM build checks;
       recovered by tools/dtor_members.py. D1 and not D2, so it is this type and not an
       inlined base. */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
};

#endif /* __cplusplus */

#endif /* DAOBJRC_DORIFU_C_H */
