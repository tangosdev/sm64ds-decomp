//cpp
#include "types.h"
// @symbol _ZN21ArmedRotatingPlatform13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ArmedRotatingPlatform.h"
struct Arg { void *m[3]; };

extern int func_ov002_020b4d58(u8 *self, struct Arg *arg);
extern struct Arg data_ov036_02113e88;

int ArmedRotatingPlatform::InitResources()
{
    return func_ov002_020b4d58(((u8 *)this), &data_ov036_02113e88);
}
