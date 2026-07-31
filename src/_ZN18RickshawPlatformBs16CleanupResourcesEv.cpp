//cpp
// @symbol _ZN18RickshawPlatformBs16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RickshawPlatformBs.h"
typedef unsigned char u8;
struct Arg { void *m[3]; };

extern int func_ov002_020b4b6c(u8 *self, struct Arg *arg);
extern struct Arg data_ov047_02112508;

int RickshawPlatformBs::CleanupResources()
{
    return func_ov002_020b4b6c(((u8 *)this), &data_ov047_02112508);
}
