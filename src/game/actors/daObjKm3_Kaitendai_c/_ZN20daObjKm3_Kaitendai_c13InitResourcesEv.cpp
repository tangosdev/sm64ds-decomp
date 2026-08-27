//cpp
// @symbol _ZN20daObjKm3_Kaitendai_c13InitResourcesEv
/* daObjKm3_Kaitendai_c::InitResources -- vtable slot 0. Picks one of two
 * spawn-parameter words by the low byte of param1 (the spawner's own argument,
 * fBase_c 0x08), then delegates to the shared ov002 setup helper with this
 * class's three-word resource descriptor. */
#include "daObjKm3_Kaitendai_c.h"

/* The descriptor the ov002 helper reads: three words, contents unrecovered. */
struct Arg { void *m[3]; };

extern "C" {
int func_ov002_020b676c(u8 *self, struct Arg *arg, s16 value);
extern s16 data_ov047_02112320;
extern s16 data_ov047_02112324;
extern struct Arg data_ov047_02112334;
}

int daObjKm3_Kaitendai_c::InitResources()
{
    s16 value = data_ov047_02112320;

    if ((param1 & 0xff) == 1) {
        value = data_ov047_02112324;
    }

    return func_ov002_020b676c((u8 *)this, &data_ov047_02112334, value);
}
