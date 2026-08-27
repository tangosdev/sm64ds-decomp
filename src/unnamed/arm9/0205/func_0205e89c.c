#include "types.h"
int func_0205e89c(u32 *ctx)
{
    if (ctx == 0) return 1;
    ctx[0x14 / 4] = 0;
    ctx[0x18 / 4] = 0;
    ctx[0x1c / 4] = 0;
    ctx[0] = 0x67452301;
    ctx[1] = 0xefcdab89;
    ctx[2] = 0x98badcfe;
    ctx[3] = 0x10325476;
    ctx[4] = 0xc3d2e1f0;
    ctx[0x60 / 4] = 0;
    ctx[0x64 / 4] = 0;
    return 0;
}
