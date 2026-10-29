#include "nitro/fx/fx.h"

extern short data_02082214[];

void func_0203cd80(int *m, short angle) {
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

void func_0203ccd4(int *m, short angle) {
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

void func_0203cc28(int *p, int angle) {
    int a = (unsigned short)angle >> 4;
    int c = data_02082214[a * 2];
    int s = data_02082214[a * 2 + 1];
    int x = p[0];
    int z = p[1];
    p[0] = FX_MUL(s, x) - FX_MUL(c, z);
    p[1] = FX_MUL(c, x) + FX_MUL(s, z);
}
