//cpp
#include "nitro/fx/fx_cp.h"

extern "C" {
extern unsigned int func_02070348(void);

/* A pseudo-random value in [0, n): n times func_02070348's 15-bit draw,
 * over 0x8000. */
int func_ov007_020c3ba8(int n) {
    int r = func_02070348();
    return FX_DivS32(n * r, 0x8000);
}
}
