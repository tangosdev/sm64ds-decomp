#include "types.h"
extern int func_02053200(int x);
struct M { int _00, _01, _10, _11; };
extern void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, struct M* a5);
extern void func_ov004_020b1cf0(int a, int b, int c, int d, int e);
extern s16 data_02082214[];
extern int data_ov006_021346bc;
void func_ov004_020b410c(char* self)
{
    char* el = self + 0x34;
    int x = *(int*)(self + 0x24);
    int idx = (int)(((unsigned int)x << 15) >> 16) >> 4;
    s16 s = *(s16*)((int)data_02082214 + (idx << 2));
    int v = func_02053200((int)((((s64)s << 11) + 0x800) >> 12) + 0x1000);
    struct M m = {0};
    u16 lim = 0xffff;
    u16 flag;
    m._00 = v;
    m._11 = v;
    do {
        func_ov004_020b1c68(el,
            *(s16*)(self + 0x10), *(s16*)(self + 0x12),
            *(int*)(self + 0x1c), *(int*)(self + 0x18), &m);
        flag = *(u16*)(el + 6);
        el += 8;
    } while (flag != lim);

    x = *(int*)(self + 0x20);
    if (x != 0 && x != 1 && x != 2)
        return;
    func_ov004_020b1cf0(data_ov006_021346bc,
        *(s16*)(self + 0x10), *(s16*)(self + 0x12),
        *(int*)(self + 0x1c), *(int*)(self + 0x18));
}
