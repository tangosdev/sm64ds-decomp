typedef unsigned char u8;
typedef unsigned long long u64;

extern void MultiStore_Int(int val, int *dst, int len);
extern u64 _ZN4cstd4sqrtEy(u64);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int, unsigned int);
extern int data_0209f608;
extern u8 data_0209f648[];
extern int data_0209f60c;

struct Px { u8 a, b, c, d; };

#pragma opt_strength_reduction off
void func_0202ee94(int *arg)
{
    int fp;
    int sl;
    int i;
    int r8, r7;
    struct Px *buf;
    int r5, r4;
    int j;
    volatile int val;

    fp = (data_0209f608 == 0) ? 1 : 0;
    val = 0x80807f7f;
    buf = (struct Px *)(data_0209f648 + fp * 0x300);

    MultiStore_Int(val, (int *)buf, 0x300);

    sl = arg[7] >> 0xc;
    r8 = 0x60 - sl;
    r7 = sl << 1;
    r5 = 0;
    r4 = 0;
    for (i = 0; i < 0x60; i++) {
        if (r8 <= i) {
            int a = i - 0x60 + sl;
            int s = (int)_ZN4cstd4sqrtEy((u64)(long long)(a * (r7 - a)));
            int lo = 0x80 - s;
            int hi = s + 0x80;
            if (lo < 0) lo = r5;
            if (hi > 0xff) hi = r4;
            buf[i].d = (u8)lo;
            buf[i].a = (u8)hi;
        }
    }

    r4 = 0x5f;
    for (j = 0; j < 0x60; j++) {
        buf[j + 0x60].a = buf[r4].a;
        buf[j + 0x60].d = buf[r4].d;
        r4--;
    }

    _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)data_0209f648, 0x600);
    data_0209f60c = fp;
}
