#pragma opt_common_subs off
#pragma opt_strength_reduction off

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

extern int WM_GetSystemWork(void);
extern int WM_CheckStateEx(int count, ...);
extern void _ZN4CP1519InvalidateDataCacheEjj(u32 addr, u32 len);
extern void MultiStore32Bytes(unsigned val, int *dst, int len);
extern int func_02061c88(int idx, int b, int c);
typedef void (*Fn)(void *);
extern int func_020623ec(Fn a0, int a1, int a2, int a3, u16 a4, u16 a5);
extern void func_02062bdc(void);
extern void func_02062d10(void *self);
extern void func_02062aa4(void);

#define H(o) (*(u16 *)(self + (o)))
#define H414_LAUNDERED \
    (*(u16 *)(((int)self + 0x414) & 0xFFFFFFFFFFFFFFFFULL))

int func_020631dc(u8 *self, int unitIdx, int mask, int elemSize, int flag)
{
    u16 f86 = 1;
    int g;
    int r;
    u16 flag5;
    u16 cnt;
    u16 j;
    u16 mbit;
    u16 bits;
    volatile unsigned zero;

    g = WM_GetSystemWork();
    r = WM_CheckStateEx(2, 9, 0xa);
    if (r != 0)
        return r;

    _ZN4CP1519InvalidateDataCacheEjj(*(u32 *)(g + 4) + 0x10, 4);
    if (*(u32 *)(*(u32 *)(g + 4) + 0x10) == 0)
        return 3;

    if (self == 0)
        return 6;
    if ((u32)unitIdx >= 0x10)
        return 6;
    if (mask == 0)
        return 6;

    _ZN4CP1519InvalidateDataCacheEjj(*(u32 *)(g + 4) + 0x184, 2);
    flag5 = *(u16 *)(*(u32 *)(g + 4) + 0x184);
    if (flag5 == 0) {
        _ZN4CP1519InvalidateDataCacheEjj(*(u32 *)(g + 4) + 0x86, 2);
        f86 = *(u16 *)(*(u32 *)(g + 4) + 0x86);
    }

    zero = 0;
    MultiStore32Bytes(zero, (int *)self, 0x420);

    H(0x408) = 0;
    H(0x40a) = 0;
    H(0x40c) = 0;
    H(0x410) = elemSize;
    H(0x416) = unitIdx;
    bits = 0;
    H(0x40e) = bits;

    H(0x418) = (flag != 0);
    mbit = 1;
    bits = (u16)(mask | (mbit << flag5));
    H(0x40e) = bits;

    cnt = 0;
    for (j = 0; j < 0x10; j++) {
        if (bits & mbit)
            cnt++;
        mbit = (u16)(mbit << 1);
    }

    H(0x412) = cnt;
    H(0x414) = (u16)(elemSize * cnt);
    if (H(0x414) > 0xfc) {
        H(0x40e) = 0;
        return 6;
    }

    H414_LAUNDERED += 4;
    H(0x41c) = 1;

    if (flag5 == 0) {
        int i;
        u8 *cur;
        int n;

        for (i = 0; i < 4; i++)
            *(u16 *)(self + i * 0x100) =
                (u16)(H(0x40e) & (f86 | 1));

        func_02061c88(unitIdx, (int)func_02062bdc, (int)self);

        cur = self;
        for (n = 0; n < ((H(0x418) == 1) ? 2 : 1); n++) {
            int cr;

            H(0x408) = (H(0x408) + 1) % 4;
            cr = func_020623ec(
                func_02062d10,
                (int)cur,
                H(0x414),
                (u16)(H(0x40e) & f86),
                H(0x416),
                1);

            if (cr == 7) {
                *(u16 *)(self + 0x400 + n * 2) = 0xffff;
                H(0x40a) = (H(0x40a) + 1) % 4;
            } else if (cr != 0 && cr != 2) {
                H(0x41c) = 3;
                return 1;
            }

            cur += 0x100;
        }
    } else {
        H(0x40a) = 3;
        func_02061c88(unitIdx, (int)func_02062aa4, (int)self);
    }

    return 0;
}
