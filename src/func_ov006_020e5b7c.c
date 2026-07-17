typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern s16 data_02082214[];

void func_02012718(int a, int b);
int func_020126e8(int a);
int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h);
void func_ov006_020e5450(char *c, int a);
void func_ov006_020e59b0(char *c, int a);

void func_ov006_020e5b7c(char *c, int idx)
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
    int p;
    int w;
    int *pd;
    int sn;
    int cs;

    m = idx * 0x30;

    pang = (u16 *)(c + 0x4686 + m);
    p668 = (int *)(c + 0x4668 + m);
    p660 = (int *)(c + 0x4660 + m);
    p664 = (int *)(c + 0x4664 + m);

    sn = data_02082214[((*pang) >> 4) * 2 + 1];
    *p660 += (int)(((long long)sn * *p668 + 0x800) >> 12);
    cs = data_02082214[((*pang) >> 4) * 2];
    *p664 += (int)(((long long)cs * *p668 + 0x800) >> 12);
    *(u16 *)(c + 0x4684 + m) += *(u16 *)((int)(((int)c + m) & 0xFFFFFFFFFFFFFFFF) + 0x4682);

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

    *(int *)(c + 0x4668 + m) -= v;
    pd = (int *)(c + 0x4668 + m);
    if (*(int *)(c + 0x4668 + m) <= 0) {
        *p668 = 0;
        *(u8 *)((int)(((long long)(int)(c + m)) & 0xFFFFFFFFFFFFFFFFLL) + 0x4688) = 2;
    }

    func_ov006_020e5450(c, idx);
    func_ov006_020e59b0(c, idx);

    v = *pd;
    w = -0xfa - ((0xc0 - (v >> 8)) * -0xfa) / 0xc0;
    p = v >> 7;
    if (p >= 0x7f)
        p = 0x7f;
    *(int *)(c + 0x467c + m) = func_02012468(*(int *)(c + 0x467c + m), 2, 0xe7, 7, p, w, func_020126e8(*p660), 0);
}
