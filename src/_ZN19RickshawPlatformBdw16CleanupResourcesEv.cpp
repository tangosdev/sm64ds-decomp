//cpp
// @symbol _ZN19RickshawPlatformBdw16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RickshawPlatformBdw.h"
typedef unsigned char u8;
struct Arg { void *m[3]; };

extern int func_ov002_020b4b6c(u8 *self, struct Arg *arg);
extern struct Arg data_ov043_02112518;

int RickshawPlatformBdw::CleanupResources()
{
    return func_ov002_020b4b6c(((u8 *)this), &data_ov043_02112518);
}
