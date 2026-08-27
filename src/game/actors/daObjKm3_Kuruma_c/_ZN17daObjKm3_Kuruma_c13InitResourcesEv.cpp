//cpp
// @symbol _ZN17daObjKm3_Kuruma_c13InitResourcesEv
/* daObjKm3_Kuruma_c::InitResources -- vtable slot 0. The whole body is a
 * delegation to the shared ov002 cart-setup helper, handing it this class's own
 * three-word resource descriptor. */
#include "daObjKm3_Kuruma_c.h"

/* The descriptor the ov002 helper reads: three words, contents unrecovered. */
struct Arg { void *m[3]; };

extern "C" {
int func_ov002_020b6958(u8 *self, struct Arg *arg);
extern struct Arg data_ov047_02112408;
}

int daObjKm3_Kuruma_c::InitResources()
{
    return func_ov002_020b6958((u8 *)this, &data_ov047_02112408);
}
