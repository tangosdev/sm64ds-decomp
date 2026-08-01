#include "types.h"
#define LM(x) ((u32*)(long long)(int)(x))

extern u8 data_0209d698;
extern u8* data_0209d6f0;
extern u8 data_0209d6a8;
extern u8 data_0209d6a4;
extern u8 data_0209d65c;
extern u8 data_0209d6a0;
extern u8 data_0209d710[];
extern volatile u32 data_0208eeac[];
extern volatile u32 data_0209d74c[];
extern volatile u32 data_0208ee8c[];

unsigned int _ZN3G2S13GetBG1CharPtrEv(void);
int func_02054d88(void);
void MultiCopy_Int(int* dst, int* src, int len);

void func_0201adfc(void)
{
    int n;
    int i;
    int j;
    u32* p;

    if (data_0209d698 != 0) {
        p = (u32*)(_ZN3G2S13GetBG1CharPtrEv() + 0x4000);
    } else {
        p = (u32*)(func_02054d88() + 0x4000);
    }

    for (i = 0; i < data_0209d6f0[6] * 2; i++) {
        for (j = 0; j < data_0209d6a8; j++) {
            MultiCopy_Int((int*)(p + 4), (int*)p, 0x10);
            if (i == data_0209d6f0[6] * 2 - 1) {
                p[4] = 0;
                p[5] = 0;
                p[6] = 0;
                p[7] = 0;
            } else {
                MultiCopy_Int((int*)(p + data_0209d6a8 * 8), (int*)(p + 4), 0x10);
            }
            p += 8;
        }
    }

    n = data_0209d6f0[6] * 2;
    if (data_0209d698 != 0) {
        p = (u32*)(_ZN3G2S13GetBG1CharPtrEv() + ((data_0209d6a8 * (n - 1) + 0x200) << 5) + 0x10);
    } else {
        p = (u32*)(func_02054d88() + ((data_0209d6a8 * (n - 1) + 0x200) << 5) + 0x10);
    }

    {
        int cnt = data_0209d6a4;
        int k;
        u8 pos = data_0209d65c;

        k = 0;
        if (cnt > 0) {
            int base = data_0209d6a0;
            u8* sp2 = data_0209d710;

            do {
                u8 bit = pos & 7;
                int word = (u32)pos >> 3;
                u32* q = &p[word << 3];
                volatile u32* mask = &data_0208eeac[bit];

                p[word << 3] |= *mask & data_0209d74c[base];
                *LM(q + 1) |= *mask & data_0209d74c[base + 1];
                *LM(q + 2) |= *mask & data_0209d74c[base + 2];
                *LM(q + 3) |= *mask & data_0209d74c[base + 3];

                if (bit != 0) {
                    volatile u32* omask = &data_0208ee8c[bit];

                    q[8] = data_0209d74c[base] & *omask;
                    q[9] = data_0209d74c[base + 1] & *omask;
                    q[10] = data_0209d74c[base + 2] & *omask;
                    q[11] = data_0209d74c[base + 3] & *omask;
                }

                pos = (u8)(pos + *sp2);
                base += 0x10;
                sp2++;
                k++;
            } while (k < cnt);
        }
    }

    data_0209d6a0 += 3;
}
