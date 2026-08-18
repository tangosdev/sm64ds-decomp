#ifndef FLOATONWATERPLATFORMJRB_H
#define FLOATONWATERPLATFORMJRB_H

#include "types.h"

/* Jolly Roger Bay's floating plank. `ita` is a board.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjFloatBoard_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjFloatBoard_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI13daObjKi_Ita_c  ov016 0x02114b80
 *   _ZTS13daObjKi_Ita_c  ov016 0x02114b98
 *   _ZTV23FloatOnWaterPlatformJrb  ov016 0x02114bcc  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjFloatBoard_c, ov002 0x02108fb4
 *
 * NO FIELDS OF ITS OWN: FloatOnWaterPlatformJrb_Spawn passes 840 = 0x348, which
 * daObjFloatBoard_c fills. It overrides slot 0 only -- the base supplies slot 3,
 * so this class has no CleanupResources.
 */

#ifdef __cplusplus

#include "daObjFloatBoard_c.h"

struct FloatOnWaterPlatformJrb : daObjFloatBoard_c {
    /* --- vtable --- */
    virtual ~FloatOnWaterPlatformJrb(); /* slots 16 (D1), 17 (D0) */

    int InitResources();               /* slot  0 */
};

typedef char FloatOnWaterPlatformJrb_size_must_be_0x348[sizeof(FloatOnWaterPlatformJrb) == 0x348 ? 1 : -1];

#endif /* __cplusplus */

#endif /* FLOATONWATERPLATFORMJRB_H */
