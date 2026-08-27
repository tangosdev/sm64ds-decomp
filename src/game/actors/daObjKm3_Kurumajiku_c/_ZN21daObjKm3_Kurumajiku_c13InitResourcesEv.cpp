//cpp
// @symbol _ZN21daObjKm3_Kurumajiku_c13InitResourcesEv
/* daObjKm3_Kurumajiku_c::InitResources -- vtable slot 0. Delegates to the shared
 * ov002 setup helper with this class's descriptor and the actor ID (0x97) of the
 * cart it drives. */
#include "daObjKm3_Kurumajiku_c.h"

/* The descriptor the ov002 helper reads: three words, contents unrecovered. */
struct Arg { void *m[3]; };

extern "C" {
int func_ov002_020b6c54(u8 *self, struct Arg *arg, unsigned int actorID);
extern struct Arg data_ov047_02112258;
}

int daObjKm3_Kurumajiku_c::InitResources()
{
    return func_ov002_020b6c54((u8 *)this, &data_ov047_02112258, 0x97);
}
