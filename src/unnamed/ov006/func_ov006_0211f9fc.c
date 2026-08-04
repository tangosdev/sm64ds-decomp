//cpp
// NONMATCHING: register allocation (div=31). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;

extern "C" void func_ov006_0211fb1c(void);
extern "C" u64 _ZN4cstd4sqrtEy(u64 x);
extern "C" int func_ov004_020ae5c4(int self, int a, int b, int c, int d, int e, int f);
extern "C" int func_020126e8(int a);
extern "C" int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h);

#define B ((char *)(self + 0x4000))

extern "C" void func_ov006_0211f9fc(int self)
{
    int flag;

    if (*(u8 *)(B + 0xc20) == 0)
        return;
    if (*(u8 *)(B + 0xc27) != 0)
        return;

    flag = 0;
    func_ov006_0211fb1c();

    if (*(u8 *)(B + 0xc1d) != 0) {
        int dx = *(s32 *)(B + 0xbf0) - *(s32 *)(B + 0xbf8);
        int dy = *(s32 *)(B + 0xbec) - *(s32 *)(B + 0xbf4);
        s64 d2 = (s64)(dy * dy + dx * dx);
        if ((s32)_ZN4cstd4sqrtEy((u64)d2) <= 3)
            flag = 1;
    }

    if (flag == 0) {
        func_ov004_020ae5c4(self, *(s32 *)(B + 0xbf4), *(s32 *)(B + 0xbf8),
                            *(s32 *)(B + 0xbec), *(s32 *)(B + 0xbf0), 0, 8);
        {
            int r = func_020126e8(*(s32 *)(B + 0xbec) << 12);
            *(s32 *)(B + 0xc08) = func_02012468(*(s32 *)(B + 0xc08), 2, 0x1f8, 4, 0, 0, r, 0);
        }
    }

    if (flag == 0) {
        *(s32 *)(B + 0xbf4) = *(s32 *)(B + 0xbec);
        *(s32 *)(B + 0xbf8) = *(s32 *)(B + 0xbf0);
    }
}
