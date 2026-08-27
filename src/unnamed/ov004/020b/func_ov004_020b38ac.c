#include "types.h"
extern int func_02053200(int x);
struct M { int _00, _01, _10, _11; };
extern void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, struct M* a5);

void func_ov004_020b38ac(char* self) {
    char* el = self + 0x34;
    int x = *(int*)(self + 0x24) >> 2;
    int t = 0x1000;
    int base = t;

    u16 flag;
    do {
        int v = base;
        int d = x - t;
        if (d < 0) d = -d;
        if (d < 0x1000) v += (int)(((s64)(0x1000 - d) * 0x400 + 0x800) >> 12);
        v = func_02053200(v);
        {
            struct M m = {0};
            m._00 = v;
            m._11 = v;
            func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                                *(int*)(self + 0x1c), *(int*)(self + 0x18), &m);
        }
        t += 0x1000;
        flag = *(u16*)(el + 6);
        el += 8;
    } while (flag != 0xffff);
}
