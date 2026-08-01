#include "types.h"
extern u16 data_02099fa4;
extern unsigned int func_02049018(int *a);
extern void func_0204f924(void *p, int v);
extern void func_0204f7cc(void *c, int h, int v);

void func_020485f0(int *thiz, int *a)
{
    int r5 = a[0] >> 12;
    unsigned int x;
    u16 r4;
    int v;
    if (thiz == 0) return;
    if (thiz[0] == 0) return;
    x = func_02049018(a);
    r4 = *(u16 *)(((char *)thiz[0]) + 0x3a);
    if (r4 == 0x81) {
        if (x > 0x4b0) v = 0;
        else if (x <= 0x64) v = 0x7f;
        else v = (int)(0x4b0 - x) * 0x7f / 1100;
    } else {
        if (x >= 0x8bc) v = 0;
        else if (x < 0x3e8) v = 0x7f;
        else v = (int)(0x8bc - x) * 0x7f / 1236;
    }
    func_0204f924(thiz, v);
    r5 = r5 >> 4;
    if (r5 > 0x3f) r5 = 0x3f;
    else if (r5 < -0x40) r5 = -0x40;
    if (r4 == 0x103) {
        func_0204f7cc(thiz, 1, r5);
    } else {
        func_0204f7cc(thiz, data_02099fa4, r5);
    }
}
