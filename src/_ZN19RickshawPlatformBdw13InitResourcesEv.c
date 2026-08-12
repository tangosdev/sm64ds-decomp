#include "types.h"
struct Arg { void *m[3]; };

extern int func_ov002_020b6958(u8 *self, struct Arg *arg);
extern struct Arg data_ov043_02112418;

int _ZN19RickshawPlatformBdw13InitResourcesEv(u8 *self)
{
    return func_ov002_020b6958(self, &data_ov043_02112418);
}
