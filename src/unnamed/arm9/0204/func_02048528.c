#include "types.h"
extern u16 data_02099fa4;
extern unsigned int func_02049018(int *a);
extern void func_0204f924(void *p, int v);
extern void func_0204f7cc(void *c, int h, int v);

void func_02048528(int *thiz, int *a)
{
    int r4 = a[0] >> 12;
    unsigned int x;
    int v;
    if (thiz == 0) return;
    if (thiz[0] == 0) return;
    x = func_02049018(a);
    if (x >= 0x1f4) {
        v = 0x1e;
    } else if (x < 0x14) {
        v = 0x7f;
    } else {
        v = (int)(0x1f4 - x) * 0x61 / 0x1e0 + 0x1e;
    }
    func_0204f924(thiz, v);
    r4 = r4 >> 2;
    if (r4 > 0x3f) r4 = 0x3f;
    else if (r4 < -0x40) r4 = -0x40;
    func_0204f7cc(thiz, data_02099fa4, r4);
}
