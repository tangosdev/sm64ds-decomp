//cpp
#include "types.h"
// @symbol _ZN19RickshawPlatformBdw13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RickshawPlatformBdw.h"
struct Arg { void *m[3]; };

extern "C" {
extern int func_ov002_020b4d58(u8 *self, struct Arg *arg);
}
extern struct Arg data_ov043_02112518;

int RickshawPlatformBdw::InitResources()
{
    return func_ov002_020b4d58(((u8 *)this), &data_ov043_02112518);
}
