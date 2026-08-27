#include "types.h"
extern u8 data_0209fcdc[];
extern u8 data_0209fc78[];
extern u8 data_0209fc88[];
extern u8 data_0208f174[];

extern void *_ZN2G213GetBG2CharPtrEv(void);
extern void *_ZN3G2S13GetBG0CharPtrEv(void);
extern int func_02054d88(void);
extern void MultiCopy_Int(int *dst, int *src, int len);

void func_020316d8(int screen, int c, u32 x, void *buf)
{
    u32 *chr;
    int pitch;
    if (screen == 1)
        chr = (u32 *)((char *)_ZN2G213GetBG2CharPtrEv() + ((data_0209fcdc[0] + 0x280 + (x >> 3)) << 5));
    else if (screen == 0)
        chr = (u32 *)((char *)_ZN3G2S13GetBG0CharPtrEv() + ((data_0209fc78[0] + 0x200 + (x >> 3)) << 5));
    else
        chr = (u32 *)((char *)func_02054d88() + ((data_0209fc78[0] + 0x200 + (x >> 3)) << 5));

    if ((x & 0xf) == 0) {
        MultiCopy_Int((int *)((char *)buf + ((((c & 0xf) << 1) + ((c & 0xf0) << 2)) << 5)),
                      (int *)chr, 0x40);
        pitch = (data_0209fc88[0] + 7) / 8;
        MultiCopy_Int((int *)((char *)buf + (((c & 0xf) + (((c & 0xf0) << 1) + 0x10)) << 6)),
                      (int *)(chr + pitch * 8), 0x40);
    } else {
        int lo;
        int hi;
        u32 *src;
        int m;
        u32 *src2;
        int idx;
        int i;
        short rem;
        short n;
        int stride;
        int k;
        int sh;
        int rsh;
        lo = c & 0xf;
        hi = c & 0xf0;
        src = (u32 *)((char *)buf + (((lo << 1) + (hi << 2)) << 5));
        src2 = src + 8;
        n = (short)(int)data_0208f174[c];
        i = 0;
        stride = (((int)data_0209fc88[0] + 7) / 8) << 3;
        m = x & 7;
        sh = m << 2;
        k = 8 - m;
        rsh = k << 2;
        for (; i < 0x10; i++) {
            idx = (i >> 3) * stride + (i & 7);
            chr[idx] |= *src << sh;
            rem = n;
            rem -= k;
            if (rem > 0) {
                (chr + idx)[8] = (*src >> rsh) | (*src2 << sh);
                rem = rem - 8;
            }
            if (rem > 0)
                *(u32 *)((char *)chr + (idx << 2) + 0x40) = *src2 >> rsh;
            src += 1;
            src2 += 1;
            if (i == 7) {
                src += 0xf8;
                src2 += 0xf8;
            }
        }
    }
}
