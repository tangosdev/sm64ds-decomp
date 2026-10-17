#include "nitro/fx/fx.h"
void func_0204c0e8(int arg0, int arg1, int *m)
{
    int r3 = FX_MUL((0x1000 - arg1), 0x555);
    int r5 = FX_MUL(arg0, 0x93d);
    int r1;
    int sum = r3 + r5;
    int dif = r3 - r5;
    r1 = r3 + arg1;
    m[0] = r1;
    m[3] = sum;
    m[6] = dif;
    m[9] = 0;
    m[1] = dif;
    m[4] = r1;
    m[7] = sum;
    m[10] = 0;
    m[2] = sum;
    m[5] = dif;
    m[8] = r1;
    m[11] = 0;
}
