//cpp
// @symbol _ZN19RickshawPlatformBdw13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RickshawPlatformBdw.h"
typedef unsigned char u8;
struct Arg { void *m[3]; };

extern int func_ov002_020b4d58(u8 *self, struct Arg *arg);
extern struct Arg data_ov043_02112518;

int RickshawPlatformBdw::InitResources()
{
    return func_ov002_020b4d58(((u8 *)this), &data_ov043_02112518);
}
