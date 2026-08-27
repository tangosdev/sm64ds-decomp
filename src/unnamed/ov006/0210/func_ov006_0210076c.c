#include "types.h"

extern void func_ov006_02100408(char *c, int a2, int a3);
extern void func_ov006_02101148(char *c, int i);
extern void func_02012718(int, int);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

#pragma opt_strength_reduction off
#pragma opt_common_subs off
void func_ov006_0210076c(char *c, int idx)
{
    int i;
    u8 (*arr)[0x40];
    u8 *slot;
    u8 st;
    int *px;
    int *py;
    int k1800;
    int kn1800;
    u8 *pcnt;
    int k2800;
    int kn2800;
    int k2000;
    int kn2000;
    int *pvx;
    u8 *row;
    u8 *pflag;
    u8 *base4k;
    int *p4664;
    int dx;
    int dy;
    int *p466c;
    int *pxrow;
    int z;

    arr = (u8 (*)[0x40])c;
    slot = arr[idx];
    st = slot[0x5296];
    if (st == 0)
        return;
    if (st >= 0xc)
        return;

    px = (int *)(slot + 0x5260);
    py = (int *)(slot + 0x5264);
    k1800 = 0x1800;
    kn1800 = -k1800;
    pcnt = slot + 0x5299;
    k2800 = 0x2800;
    kn2800 = -k2800;
    k2000 = 0x2000;
    kn2000 = -k2000;
    i = 0;
    pvx = (int *)(slot + 0x5268);
    z = i;

    do {
        row = arr[i];
        pflag = row + 0x4698;
        if (*pflag != 0) {
            base4k = row + 0x4000;
            if (base4k[0x699] >= 2) {
                p466c = (int *)(row + 0x466c);
                if (*p466c > 0x2000) {
                    dx = (*px - *(int *)(row + 0x4660)) >> 12;
                    p4664 = (int *)(row + 0x4664);
                    dy = (*py - *p4664) >> 12;
                    if (dx >= -0x30 && dx <= 0x30 && dy >= 0 && dy <= 8) {
                        if (dx >= -0x10 && dx <= 0x10) {
                            *pcnt = (u8)(*pcnt + 1);
                            row[0x469a] = (u8)z;
                            pxrow = (int *)(row + 0x4660);
                            *pflag = (u8)z;
                            func_ov006_02100408(c, *pxrow, *p4664);
                            func_02012718(0x19a, *pxrow);
                            func_ov006_02101148(c, idx);
                        } else if (dx >= -0x20 && dx <= 0x20) {
                            if (dx < 0)
                                *(int *)(arr[i] + 0x4668) = *pvx - 0x1800;
                            else
                                *(int *)(arr[i] + 0x4668) = *pvx + 0x1800;
                            *p466c = kn2000;
                            _ZN5Sound12PlayBank2_2DEj(0x19b);
                        } else {
                            if (dx < 0)
                                *(int *)(arr[i] + 0x4668) = k1800;
                            else
                                *(int *)(arr[i] + 0x4668) = kn1800;
                            *p466c = kn2800;
                            _ZN5Sound12PlayBank2_2DEj(0x19b);
                        }
                    }
                }
            }
        }
        i++;
    } while (i < 0x30);
}
