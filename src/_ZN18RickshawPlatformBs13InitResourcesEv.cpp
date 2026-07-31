//cpp
#include "types.h"
// @symbol _ZN18RickshawPlatformBs13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RickshawPlatformBs.h"
struct Arg { void *m[3]; };

extern int func_ov002_020b4d58(u8 *self, struct Arg *arg);
extern struct Arg data_ov047_02112508;

int RickshawPlatformBs::InitResources()
{
    return func_ov002_020b4d58(((u8 *)this), &data_ov047_02112508);
}
