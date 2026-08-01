#include "types.h"
extern void func_ov004_020adb1c(int self);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern char *func_020beb68;

void func_ov006_020f0274(char *s)
{
    if (*(u8 *)(s + 0x47e0) == 0)
        return;

    if (*(u8 *)(s + 0x47e1) == 0) {
        *(int *)(((int)s + 0x47d4)) += *(int *)(s + 0x47d8);
        *(int *)(((int)s + 0x47d8)) -= 0x100;
        if (*(u8 *)(s + 0x47df) != 0) {
            int v;
            (*(u8 *)(((int)s + 0x47df)))--;
            v = *(u8 *)(s + 0x47df);
            if (v < 0)
                *(u8 *)(s + 0x47df) = 0;
            return;
        }
        *(u8 *)(s + 0x47df) = 0x40;
        *(u8 *)(((int)s + 0x47e1)) += 1;
        return;
    }

    if (*(u8 *)(s + 0x47e1) == 1) {
        if (*(u8 *)(s + 0x47df) != 0) {
            int v;
            (*(u8 *)(((int)s + 0x47df)))--;
            v = *(u8 *)(s + 0x47df);
            if (v < 0)
                *(u8 *)(s + 0x47df) = 0;
            return;
        }
        *(u8 *)(s + 0x47e3) = 0;
        *(u8 *)(((int)s + 0x47e1)) += 1;
        {
            char *g = func_020beb68;
            if (g != 0) {
                if (*(int *)(g + 0xb4) < 0x270f)
                    *(int *)(((int)g + 0xb4)) += 1;
                if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                    *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
            }
        }
        func_ov004_020adb1c(func_020beb68 != 0 ? *(int *)(func_020beb68 + 0xb4) : 0);
        return;
    }

    if (*(u8 *)(s + 0x47e2) != 0) {
        *(u8 *)(((int)s + 0x47df)) += 1;
        if (*(u8 *)(s + 0x47df) < 4)
            return;
        _ZN5Sound12PlayBank2_2DEj(0x1bc);
        *(u8 *)(s + 0x47df) = 0;
        *(u8 *)(((int)s + 0x47e2)) -= 1;
        *(u16 *)(((int)s + 0x5172)) += 1;
        if (*(u16 *)(s + 0x5172) >= 0x32) {
            *(u16 *)(s + 0x5172) = 0x32;
            *(u8 *)(s + 0x51fb) = 0;
        }
        return;
    }

    *(u8 *)(s + 0x47e0) = 0;
}
