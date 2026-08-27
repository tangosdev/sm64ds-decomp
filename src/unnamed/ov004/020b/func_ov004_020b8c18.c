#include "types.h"
extern u16 *_ZN3G2S12GetBG1ScrPtrEv(void);
extern void func_ov004_020aea78(void *self, int a1, int a2, int a3);

extern u16 data_ov004_020bca58[];
extern u16 data_ov004_020bca60[];

void func_ov004_020b8c18(char *self)
{
    volatile s16 xy[2];
    void *tbl;
    u16 *p;
    u16 v;
    int i;
    int w;
    int mask;
    int row;
    int col;
    int y;
    int h;
    int x;

    xy[0] = *(s16 *)(self + 4);
    xy[1] = *(s16 *)(self + 6);

    switch (*(int *)(self + 0x1c)) {
    case 2:
        if (*(u8 *)(self + 0x10) != 0)
            xy[0] = (s16)(xy[0] - 2);
        tbl = data_ov004_020bca58;
        break;

    case 3:
        if (*(u8 *)(self + 0x10) != 0)
            xy[0] = (s16)(xy[0] + 2);
        tbl = data_ov004_020bca60;
        break;

    case 0:
        p = (u16 *)((char *)_ZN3G2S12GetBG1ScrPtrEv() + 0x19e);
        v = (u16)((*(int *)(self + 0x20) << 6) + 0x13);
        for (i = 0; i < 13; i++) {
            p[0] = v;
            p[0x20] = (u16)(v + 0x20);
            p++;
            v++;
        }
        /* fallthrough */
    case 1:
    case 6:
        mask = (*(u8 *)(self + 0x10) != 0) ? 0 : 0x3000;
        w = *(s16 *)(self + 8);
        p = _ZN3G2S12GetBG1ScrPtrEv();
        x = *(s16 *)(self + 4);
        h = *(s16 *)(self + 0xa);
        x -= w;
        x >>= 3;
        p += x;
        y = *(s16 *)(self + 6);
        y -= h;
        p += (y >> 3) << 5;
        for (row = 0; row < *(s16 *)(self + 0xa) >> 2; row++) {
            for (col = 0; col < w >> 2; col++) {
                p[col] = (u16)(mask | (p[col] & 0xfff));
                w = *(s16 *)(self + 8);
            }
            p += 0x20;
        }
        return;

    default:
        return;
    }

    func_ov004_020aea78(tbl, xy[0], xy[1], 0);
}
