//cpp
#include "types.h"
// @symbol _ZN21ArmedRotatingPlatform16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ArmedRotatingPlatform.h"
struct Arg { void *m[3]; };

extern int func_ov002_020b4b6c(u8 *self, struct Arg *arg);
extern struct Arg data_ov036_02113e88;

int ArmedRotatingPlatform::CleanupResources()
{
    return func_ov002_020b4b6c(((u8 *)this), &data_ov036_02113e88);
}
