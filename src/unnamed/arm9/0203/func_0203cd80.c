#include "nitro/fx/fx.h"
extern short data_02082214[];

void func_0203cd80(int *m, short angle)
{
    unsigned int i = (unsigned short)angle >> 4;
    short s = data_02082214[i * 2];
    short c = data_02082214[i * 2 + 1];
    int v2 = m[2];
    int v1 = m[1];
    int o1 = FX_MUL(c, v1) - FX_MUL(s, v2);
    int o2 = FX_MUL(s, v1) + FX_MUL(c, v2);
    m[1] = o1;
    m[2] = o2;
}
