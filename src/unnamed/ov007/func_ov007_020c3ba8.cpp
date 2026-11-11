//cpp
#include "nitro/fx/fx_cp.h"
#include "msl/stdlib.h"

extern "C" {
/* A pseudo-random value in [0, n): n times MSL rand's 15-bit draw,
 * over 0x8000. */
int func_ov007_020c3ba8(int n) {
    int r = rand();
    return FX_DivS32(n * r, 0x8000);
}
}
