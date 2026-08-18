#include "common.h"

#pragma opt_common_subs off

extern int *_ZN3G2S13GetBG0CharPtrEv(void);

void func_ov006_0211ea70(char *self, int idx)
{
    int gx, gy;
    int cnt;
    int y;
    int x;
    int *pA;
    int *pB;
    char *tile;
    int tidx;
    int *row;
    int word;
    int off;

    off = idx * 0x24;
    cnt = 0;
    pA = (int *)(self + off + 0x4660);
    pB = (int *)(self + off + 0x4664);
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            gx = x + ((*pA >> 12) - 4);
            gy = y + ((*pB >> 12) - 4);
            tile = (char *)_ZN3G2S13GetBG0CharPtrEv();
            tidx = (gx >> 3) + ((gy >> 3) << 5);
            row = (int *)(tile + (tidx << 5));
            /* pin row complete before nibble extract */
            row = (int *)((char *)row + (gy - gy));
            word = row[gy & 7];
            if (((word >> ((gx & 7) << 2)) & 0xf) == 0)
                cnt++;
        }
    }
    if (cnt == 0)
        return;
    if (*(u8 *)(self + off + 0x467f) == 0)
        return;
    *(u16 *)(self + 0x466c + idx * 0x24) += 0x8000;
}
