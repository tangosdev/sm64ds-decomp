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
 * dBgW_KcMbg[5] in between. That shape is inherited from the base declaration,
 * and IS compiler-derived: the inline empty body below is enough for mwccarm to
 * generate the whole thing (see the class-body comment for why it must stay
 * inline).
 *
 *   _ZTI16daObjRc_Dorifu_c  ov036 0x02113e4c
 *   _ZTS16daObjRc_Dorifu_c  ov036 0x02113e58
 *   _ZTV16daObjRc_Dorifu_c  ov036 0x02113ecc  (its record sits at V-4)
 *   kind                    __si_class_type_info, ONE base, offset 0
 *   base                    daObjDorifu_c, ov002 0x02108d70 -- CROSSING OVERLAYS,
 *                           which is ordinary here: 183 of the ROM's 413 base
 *                           edges leave their overlay.
 *
 * NO FIELDS OF ITS OWN: daObjRc_Dorifu_c_classInit (historical alias
 * TrickyTriangles_Spawn) passes 0xdcc to
 * fBase_c::operator new, which daObjDorifu_c fills. It overrides slots 0 and 3,
 * which the base leaves null, and both bodies just forward to daObjDorifu_c's
 * shared helpers in ov002 with this class's own argument block. Everything this
 * header used to restate was dBgActor_c's, twice removed.
 */

#ifdef __cplusplus

#include "daObjDorifu_c.h"

struct daObjRc_Dorifu_c : daObjDorifu_c {
    /* --- vtable --- */
    /* MEASURED -- INLINE ON PURPOSE, do not move out of line.
     * Out of line, mwccarm 2004/b56 emits D0 before D1 (the reverse of the
     * cartridge's 0x02111f8c D1 / 0x0211200c D0) and a homeless D2, and
     * objisolate rejects the whole translation unit -- `tubuild verify`
     * reported exactly that as `1 ordinal pair(s) NOT in ROM order: [(0, 1)]`.
     * Defined in the class body it emits D1 then D0 and no D2, which is the
     * ROM's own order, and it becomes this class's key function: mwccarm
     * homes _ZTI/_ZTS/_ZTV16daObjRc_Dorifu_c in the TU that defines it rather
     * than emitting them as unhomed vague-linkage passengers. Declared FIRST
     * (this class's sibling daObjKm1_Dorifu_c and daObjWanwanShutter_c both
     * do the same) so it becomes the earliest-declared virtual, which is what
     * makes it the key function under the Itanium ABI. */
    virtual ~daObjRc_Dorifu_c() {}      /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char daObjRc_Dorifu_c_size_must_be_0xdcc[sizeof(daObjRc_Dorifu_c) == 0xdcc ? 1 : -1];

#else

/* The C spelling of the same object, flat. NOTHING IN THE TREE COMPILES THIS
   BRANCH any more: the per-function shards this was written for are gone, and
   the one file that includes this header,
   src/actors/d_a_obj_rc_dorifu.cpp, is //cpp. It is kept only as the readable
   field record -- the same arrangement as include/dBgActor_c.h -- and must stay
   in step with the C++ struct above if either changes. */
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
