#include "common.h"

#pragma opt_common_subs off

extern void func_ov006_0211fb1c(char *c);
extern int func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern s32 _ZN4cstd4sqrtEy(u64);

void func_ov006_0211f9fc(int self)
{
    int close;

    if (((u8 *)self + 0x4000)[0xC20] == 0)
        return;
    if (((u8 *)self + 0x4000)[0xC27] != 0)
        return;

    close = 0;
    func_ov006_0211fb1c((char *)self);

    if (((u8 *)self + 0x4000)[0xC1D] != 0) {
        Vector3 d;
        int dist;
        d.x = *(int *)((char *)self + 0x4000 + 0xBEC) - *(int *)((char *)self + 0x4000 + 0xBF4);
        d.y = *(int *)((char *)self + 0x4000 + 0xBF0) - *(int *)((char *)self + 0x4000 + 0xBF8);
        dist = _ZN4cstd4sqrtEy((u64)((d.x) * (d.x) + (d.y) * (d.y)));
        if (dist <= 3)
            close = 1;
        if (close == 0) {
            int snd;
            int cur;
            func_ov004_020ae5c4(
                (void *)self,
                *(int *)((char *)self + 0x4000 + 0xBF4),
                *(int *)((char *)self + 0x4000 + 0xBF8),
                *(int *)((char *)self + 0x4000 + 0xBEC),
                *(int *)((char *)self + 0x4000 + 0xBF0),
                0,
                8);
            snd = func_020126e8(*(int *)((char *)self + 0x4000 + 0xBEC) << 12);
            cur = *(int *)((char *)self + 0x4000 + 0xC08);
            *(int *)((char *)self + 0x4000 + 0xC08) = func_02012468(
                cur,
                2,
                0x1F8,
                4,
                0,
                0,
                snd,
                0);
        }
    }
    if (close == 0) {
        *(int *)((char *)self + 0x4000 + 0xBF4) = *(int *)((char *)self + 0x4000 + 0xBEC);
        *(int *)((char *)self + 0x4000 + 0xBF8) = *(int *)((char *)self + 0x4000 + 0xBF0);
    }
}
