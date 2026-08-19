//cpp
// NONMATCHING: size-exact div=26 (was 29). Residual r1↔r3 on angle/i plus delayed lsl#16 after add#0xa.
#include "types.h"
extern "C" {
u16 *_ZN2G212GetBG1ScrPtrEv(void);
int func_02030958(int val);
}
extern u8 data_0209fc50;

extern "C" unsigned int func_ov075_02116128(int unused, int idx, int flag)
{
    int b = data_0209fc50;
    u16 *bg = _ZN2G212GetBG1ScrPtrEv();
    int a = func_02030958(idx);
    int t = 4 - b;
    unsigned int angle, hi, lo;
    int i;
    u16 *p;
    int m = idx * (t * 0x20 + 0xa0) + (t * 0x40 + 0x84);
    p = bg + m;
    angle = (unsigned short)(a + 0xa);
    if (flag != 0)
        angle = (unsigned short)(angle - 4);
    hi = angle << 0x1c;
    p[0x20] = (u16)((p[0x20] & 0xfff) | (hi >> 16));
    lo = hi >> 0x10;
    i = 0;
    p[0x40] = (u16)((p[0x40] & 0xfff) | (hi >> 16));
    p[0x37] = (u16)((p[0x37] & 0xfff) | (hi >> 16));
    p[0x57] = (u16)((p[0x57] & 0xfff) | (hi >> 16));
    do {
        i++;
        p[0] = (u16)((p[0] & 0xfff) | lo);
        p[0x60] = (u16)((p[0x60] & 0xfff) | lo);
        p++;
    } while (i < 0x18);
    return lo;
}
