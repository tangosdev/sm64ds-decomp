#include "types.h"
extern void _Z14ApproachLinearRiii(int *p, int target, int step);
extern void func_ov004_020adb1c(int self);
extern char *func_020beb68;
extern char data_ov006_02141fe4[];

typedef struct {
    u8 t;
    u8 pad[0x13];
} Timer20;
extern Timer20 data_ov006_02141ff0[];

void func_ov006_020ea3d0(int *arg)
{
    int i;
    char *f = data_ov006_02141fe4;
    for (i = 0; i < 1; i++, f += 0x14) {
        u8 ph;
        if (*(u8 *)(f + 0xd) != 1)
            continue;
        if (*(int *)(f + 4) < 0x8000)
            *(int *)(f + 4) = 0x8000;
        ph = *(u8 *)(f + 0xe);
        if (ph == 0) {
            *(int *)(((int)f + 4)) += *(int *)(f + 8);
            *(int *)(((int)f + 8)) -= 0x100;
            if (*(u8 *)(f + 0xc) != 0) {
                int v;
                data_ov006_02141ff0[i].t--;
                v = data_ov006_02141ff0[i].t;
                if (v < 0)
                    data_ov006_02141ff0[i].t = 0;
                return;
            }
            *(u8 *)(f + 0xc) = 0x40;
            *(u8 *)(((int)f + 0xe)) += 1;
        } else if (ph == 1) {
            if (*(u8 *)(f + 0xc) != 0) {
                int v;
                data_ov006_02141ff0[i].t--;
                v = data_ov006_02141ff0[i].t;
                if (v < 0)
                    data_ov006_02141ff0[i].t = 0;
                return;
            }
            *(u8 *)(f + 0x10) = 0;
            *(u8 *)(((int)f + 0xe)) += 1;
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
        } else {
            if (*(u8 *)(f + 0xf) != 0) {
                *(u8 *)(((int)f + 0xc)) += 1;
                if (*(u8 *)(f + 0xc) >= 8) {
                    *(u8 *)(f + 0xc) = 0;
                    *(u8 *)(((int)f + 0xf)) -= 1;
                    if (arg != 0)
                        _Z14ApproachLinearRiii(arg, 0x32, 1);
                }
            } else {
                *(u8 *)(f + 0xd) = 0;
            }
        }
    }
}
