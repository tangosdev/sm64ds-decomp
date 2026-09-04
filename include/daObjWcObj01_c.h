#ifndef DAOBJWCOBJ01_C_H
#define DAOBJWCOBJ01_C_H

#include "types.h"

/* Wet-Dry World's square floating board.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjFloatBoard_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjFloatBoard_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI14daObjWcObj01_c  ov029 0x02113bdc
 *   _ZTS14daObjWcObj01_c  ov029 0x02113bf4
 *   _ZTV14daObjWcObj01_c  ov029 0x02113c2c  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjFloatBoard_c, ov002 0x02108fb4
 *
 * NO FIELDS OF ITS OWN: daObjWcObj01_c_classInit passes 840 = 0x348, which
 * daObjFloatBoard_c fills. It overrides slot 0 only.
 */

#ifdef __cplusplus

#include "daObjFloatBoard_c.h"

struct daObjWcObj01_c : daObjFloatBoard_c {
    /* --- vtable --- */
    /* Slots 16 (D1) and 17 (D0). DEFINED INLINE ON PURPOSE. Written
     * out-of-line in the TU, mwccarm emits D0 before D1 -- the reverse of
     * the cartridge's order -- which objisolate refuses for the whole
     * translation unit, and it emits a third D2 body with no ROM home.
     * Defined here the compiler produces the retail D1/D0 pair in ROM
     * order and no D2, while InitResources -- which overrides a base
     * virtual and so is virtual itself, and is defined out-of-line -- keeps
     * src/actors/d_a_obj_wc_obj01.cpp as this class's key-function TU.
     * The body is empty because nothing in the chain owns a destructible
     * member; the three vptr stores the ROM's D1 makes are the compiler's,
     * not the body's.  */
    virtual ~daObjWcObj01_c() {}

    int InitResources();               /* slot  0 */
};

typedef char daObjWcObj01_c_size_must_be_0x348[sizeof(daObjWcObj01_c) == 0x348 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJWCOBJ01_C_H */
