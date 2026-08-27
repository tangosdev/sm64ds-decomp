//cpp
// @symbol _ZN20daObjKm3_Kaitendai_c16CleanupResourcesEv
/* daObjKm3_Kaitendai_c::CleanupResources -- vtable slot 3. The teardown half of
 * the InitResources delegation, over the same descriptor. */
#include "daObjKm3_Kaitendai_c.h"

/* The descriptor the ov002 helper reads: three words, contents unrecovered. */
struct Arg { void *m[3]; };

extern "C" {
int func_ov002_020b66a8(u8 *self, struct Arg *arg);
extern struct Arg data_ov047_02112334;
}

int daObjKm3_Kaitendai_c::CleanupResources()
{
    return func_ov002_020b66a8((u8 *)this, &data_ov047_02112334);
}
