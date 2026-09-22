#include "types.h"
extern s16 data_02082214[];
extern int data_ov006_0212e478[];
extern int data_ov006_0212e48c[];
extern int data_ov006_0212e4a0[];
extern int data_ov006_0212e4b4[];
extern int data_ov006_0212e4c8[];

void func_02012718(int a, int b);
int func_020126e8(int a);
int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h);
void func_ov006_020e20bc(char *c, int a);
void func_ov006_020e269c(char *c, int a);

void func_ov006_020e2868(char *c, int idx)
{
    int m;
    int zi;
    u16 *pang;
    int *p668;
    int *p660;
    int *p664;
    int x;
    int z;
    int xi;
    int v;
    int sbv;
    int i2;
    int s;
    int p;
    int w;
    int *pd;
    int sn;
    int cs;

    m = idx * 0x2c;

    pang = (u16 *)(c + 0x4686 + m);
    p668 = (int *)(c + 0x4668 + m);
    p660 = (int *)(c + 0x4660 + m);
    p664 = (int *)(c + 0x4664 + m);

    sn = data_02082214[((*pang) >> 4) * 2 + 1];
    *p660 += (int)(((long long)sn * *p668 + 0x800) >> 12);
    cs = data_02082214[((*pang) >> 4) * 2];
    *p664 += (int)(((long long)cs * *p668 + 0x800) >> 12);
    *(u16 *)(c + 0x4684 + m) += *(u16 *)(c + m + 0x4682);

    x = *p660;
    z = *p664;
    xi = x >> 12;
    zi = z >> 12;

    if (xi + 0xc >= 0x100) {
        *pang = 0x8000 - *pang;
        *p660 = 0xf4000;
        func_02012718(0x1d4, *p660);
    } else if (xi - 0xc < 0) {
        *pang = 0x8000 - *pang;
        *p660 = 0xc000;
        func_02012718(0x1d4, *p660);
    }

    if (zi + 0xc > 0xc0) {
        *(u16 *)(c + 0x4686 + m) = -*(u16 *)(c + 0x4686 + m);
        *p664 = 0xb4000;
        func_02012718(0x1d4, *p660);
    } else if (zi - 0xc < -0xe0) {
        *(u16 *)(c + 0x4686 + m) = -*(u16 *)(c + 0x4686 + m);
        *p664 = -0xd4000;
        func_02012718(0x1d4, *p660);
    }

    zi = *p668;
    v = zi >> 9;
    if (v <= 0x1c)
        v = 0x1c;
    for (i2 = 0, sbv = *p664 >> 12; i2 < 5; i2++) {
        if (sbv <= -(data_ov006_0212e478[i2] + 0x20)) {
            s = data_02082214[(*pang >> 4) * 2];
            if (s < 0) {
                v = zi >> data_ov006_0212e48c[i2];
                if (v < data_ov006_0212e4b4[i2])
                    v = data_ov006_0212e4b4[i2];
            } else if (s > 0) {
                v = zi >> data_ov006_0212e4a0[i2];
                if (v < data_ov006_0212e4c8[i2])
                    v = data_ov006_0212e4c8[i2];
            }
            break;
        }
    }

    if (sbv > -0x20) {
        s = data_02082214[(*pang >> 4) * 2];
        if (s > 0) {
            v = zi >> 3;
            if (v <= 0x180)
                v = 0x180;
        }
    }

    *(int *)(c + 0x4668 + m) -= v;
    pd = (int *)(c + 0x4668 + m);
    if (*(int *)(c + 0x4668 + m) <= 0) {
        *p668 = 0;
        *(u8 *)(c + idx * 0x2c + 0x4688) = 2;
    }

    func_ov006_020e20bc(c, idx);
    func_ov006_020e269c(c, idx);

    v = *pd;
    w = -0xfa - ((0xc0 - (v >> 8)) * -0xfa) / 0xc0;
    p = v >> 7;
    if (p >= 0x7f)
        p = 0x7f;
    *(int *)(c + 0x467c + m) = func_02012468(*(int *)(c + 0x467c + m), 2, 0xe7, 7, p, w, func_020126e8(*p660), 0);
}
