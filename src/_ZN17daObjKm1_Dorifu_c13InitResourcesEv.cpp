//cpp
#include "types.h"
// @symbol _ZN17daObjKm1_Dorifu_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "daObjKm1_Dorifu_c.h"
struct Arg { void *m[3]; };

extern "C" {
extern int func_ov002_020b4d58(u8 *self, struct Arg *arg);
}
extern struct Arg data_ov043_02112518;

int daObjKm1_Dorifu_c::InitResources()
{
    return func_ov002_020b4d58(((u8 *)this), &data_ov043_02112518);
}
