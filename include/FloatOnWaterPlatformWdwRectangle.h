#ifndef FLOATONWATERPLATFORMWDWRECTANGLE_H
#define FLOATONWATERPLATFORMWDWRECTANGLE_H

#include "types.h"

/* Wet-Dry World's long floating board.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjFloatBoard_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjFloatBoard_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI14daObjWcObj06_c  ov029 0x02113ef4
 *   _ZTS14daObjWcObj06_c  ov029 0x02113f0c
 *   _ZTV32FloatOnWaterPlatformWdwRectangle  ov029 0x02113f44  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjFloatBoard_c, ov002 0x02108fb4
 *
 * NO FIELDS OF ITS OWN: FloatOnWaterPlatformWdwRectangle_Spawn passes 840 = 0x348,
 * which daObjFloatBoard_c fills. It overrides slot 0 only.
 */

#ifdef __cplusplus

#include "daObjFloatBoard_c.h"

struct FloatOnWaterPlatformWdwRectangle : daObjFloatBoard_c {
    /* --- vtable --- */
    virtual ~FloatOnWaterPlatformWdwRectangle() {} /* slots 16 (D1), 17 (D0) */

    int InitResources();               /* slot  0 */
};

typedef char FloatOnWaterPlatformWdwRectangle_size_must_be_0x348[sizeof(FloatOnWaterPlatformWdwRectangle) == 0x348 ? 1 : -1];

#endif /* __cplusplus */

#endif /* FLOATONWATERPLATFORMWDWRECTANGLE_H */
