#ifndef DAOBJCVSHUTTER_C_H
#define DAOBJCVSHUTTER_C_H

#include "types.h"

/* Hazy Maze Cave's switch-operated shutter. `cv` is the ROM's tag for that stage.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjSwdoor_c, which derives
 * from dBgActor_c. Its destructor stores THREE vptrs -- its own, daObjSwdoor_c's,
 * then dBgActor_c's. A one-level chain emits two.
 *
 *   _ZTI16daObjCvShutter_c  ov021 0x021148c4
 *   _ZTS16daObjCvShutter_c  ov021 0x021148dc
 *   _ZTV16daObjCvShutter_c        ov021 0x02114914  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjSwdoor_c, ov002 0x021099c0
 *
 * SIZE 0x324, from daObjCvShutter_c_classInit's literal 804 -- the same as daObjBSwdoor_c's, and
 * the two agreeing is what fixes the size for the pair. It overrides slots 0, 3
 * and 6, all three of which the base leaves null.
 *
 * THE FOUR BYTES AT 0x320 ARE UNOBSERVED, on the same reading as daObjBSwdoor_c's.
 *
 * WHAT THIS HEADER USED TO SAY: a `u8 mModel` marker at 0xd4 and a
 * `u8 mMovingMeshCollider` marker at 0x124. Both were dBgActor_c's, twice removed,
 * and are inherited now.
 */

#ifdef __cplusplus

#include "daObjSwdoor_c.h"

struct daObjCvShutter_c : daObjSwdoor_c {
    u8  pad_320[0x4];

    /* --- vtable --- */
    /* MEASURED -- INLINE ON PURPOSE. The class TU is the only place these
       two are emitted; with the body out of line mwcc emits D0 ahead of D1
       and the ROM has D1 first (rombuild refuses the object outright). An
       inline body also drops the D2 variant the cartridge never carried. */
    virtual ~daObjCvShutter_c() {}     /* slots 16 (D1), 17 (D0) */

    int Behavior();                    /* slot  6 */
    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char daObjCvShutter_c_size_must_be_0x324[sizeof(daObjCvShutter_c) == 0x324 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJCVSHUTTER_C_H */
