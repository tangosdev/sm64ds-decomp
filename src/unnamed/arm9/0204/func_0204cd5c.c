#include "nitro/fx/fx.h"
typedef short s16;
typedef long long s64;

void func_0204cd5c(int *out, int *in, int sel, s16 *m)
{
    switch (sel) {
    case 2:
        out[0] = 0;
        out[1] = in[0];
        out[2] = in[1];
        break;
    case 1:
        out[0] = in[1];
        out[1] = 0;
        out[2] = in[0];
        break;
    case 0:
        out[0] = in[0];
        out[1] = in[1];
        out[2] = 0;
        break;
    case 3:
        out[0] = FX_MUL(in[0], m[0x2e]) + FX_MUL(in[1], m[0x31]);
        out[1] = FX_MUL(in[0], m[0x2f]) + FX_MUL(in[1], m[0x32]);
        out[2] = FX_MUL(in[0], m[0x30]) + FX_MUL(in[1], m[0x33]);
        break;
    }
}
