#include "types.h"
extern int data_02099fa8;
extern int data_02099fac;
extern u16 data_02099fa4;
int func_02049018(void* p);
void func_0204f924(void* obj, int v);
void func_0204f7cc(void* obj, int a, int b);

void func_02048d80(void* obj, int* p)
{
    int s = p[0] >> 12;
    int hi = data_02099fac;
    int lo = data_02099fa8;
    int r1;
    int cur;
    if (obj == 0) return;
    if (*(int*)obj == 0) return;
    cur = func_02049018(p);
    if ((unsigned)cur >= (unsigned)hi) return;
    if ((unsigned)lo > (unsigned)cur)
        r1 = 0x7f;
    else
        r1 = (hi - cur) * 0x7f / (hi - lo);
    func_0204f924(obj, r1);
    s = s / 2;
    if (s > 0x3f) s = 0x3f;
    else if (s < -0x40) s = -0x40;
    func_0204f7cc(obj, data_02099fa4, s);
}
