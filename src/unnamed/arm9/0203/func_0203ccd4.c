#include "nitro/fx/fx.h"
extern short data_02082214[];

void func_0203ccd4(int *m, short angle)
{
    unsigned int i = (unsigned short)angle >> 4;
    short s = data_02082214[i * 2];
    short c = data_02082214[i * 2 + 1];
    int x = m[0];
    int z = m[2];
    int nx = FX_MUL(c, x) + FX_MUL(s, z);
    int nz = FX_MUL(c, z) - FX_MUL(s, x);
    m[0] = nx;
    m[2] = nz;
}
