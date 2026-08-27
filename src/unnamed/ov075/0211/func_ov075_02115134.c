#include "types.h"
extern int func_ov075_02115290(int dummy, int col);
extern void func_ov075_021149d0(char* dst, int val);
extern void func_02012790(int x);

extern u8 data_0209fc50[];

void func_ov075_02115134(char* c)
{
    s32 i;
    char* p = c + 0x920;
    u8 lim, cur;
    for (i = 0; i < 4; i++) {
        func_ov075_021149d0(p, func_ov075_02115290((int)c, i));
        p += 0x158;
    }
    lim = data_0209fc50[0];
    cur = *(u8*)(c + 0xf43);
    if (cur > lim)
        func_02012790(0x12a);
    else if (cur < lim)
        func_02012790(0x129);
    *(u8*)(c + 0xf43) = lim;
}
