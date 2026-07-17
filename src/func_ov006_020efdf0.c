typedef unsigned char u8;
typedef long long s64;

#pragma opt_strength_reduction off

struct Px {
    u8 a, b, c2, d2;
};

extern int data_0209f608;
extern struct Px data_0209f648[][0xc0];
extern int data_0209f60c;
extern int _ZN4cstd4sqrtEy(unsigned long long);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *p, unsigned int len);

void func_ov006_020efdf0(char *o, int i)
{
    int toggle;
    int zc = 0;
    int y;
    int k, n, m;
    u8 tmp;

    if (data_0209f608 == 0)
        toggle = 1;
    else
        toggle = 0;
    y = *(int *)(o + i * 0x14 + 0x47ec) >> 12;

    for (k = 0; k < 0x60; k++) {
        if (0x60 - y <= k) {
            int t = k - 0x60 + y;
            int s = _ZN4cstd4sqrtEy((s64)(t * (y * 2 - t)));
            int lo = (*(int *)(o + 0x47e4) >> 12) - s;
            int hi = s + (*(int *)(o + 0x47e4) >> 12);
            if (lo < 0)
                lo = zc;
            if (hi >= 0xff)
                hi = 0xff;
            data_0209f648[toggle][k].b = lo;
            data_0209f648[toggle][k].a = hi;
        } else {
            data_0209f648[toggle][k].a = 0;
            data_0209f648[toggle][k].b = 0;
        }
    }
    for (n = 0x5f, m = 0; m < 0x60; m++, n--) {
        tmp = data_0209f648[toggle][n].b;
        data_0209f648[toggle][m + 0x60].b = tmp;
        data_0209f648[toggle][m + 0x60].a = data_0209f648[toggle][n].a;
    }
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(data_0209f648, 0x600);
    data_0209f60c = toggle;
}
