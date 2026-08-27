//cpp
// @symbol _ZN17daObjKm3_Kuruma_c16CleanupResourcesEv
/* daObjKm3_Kuruma_c::CleanupResources -- vtable slot 3. The teardown half of
 * the InitResources delegation, over the same descriptor. */
#include "daObjKm3_Kuruma_c.h"

/* The descriptor the ov002 helper reads: three words, contents unrecovered. */
struct Arg { void *m[3]; };

extern "C" {
int func_ov002_020b68b0(u8 *self, struct Arg *arg);
extern struct Arg data_ov047_02112408;
}

int daObjKm3_Kuruma_c::CleanupResources()
{
    return func_ov002_020b68b0((u8 *)this, &data_ov047_02112408);
}
