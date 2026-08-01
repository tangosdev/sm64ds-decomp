#include "types.h"
extern int func_ov004_020b04c0(void);
extern void func_ov004_020b4b84(char* c, int* in);
extern void func_ov004_020b5368(void);

extern char data_ov004_020bfa34;
extern int data_ov004_020bf9f8;
extern int data_ov004_020bfa04;
extern void* data_ov004_020bfa20;

void func_ov004_020b53f0(void)
{
    int step, i, idx, n, limit;
    char* e;
    int sp[2];

    e = &data_ov004_020bfa34;
    for (i = 0; i < 0x14; i++) {
        if (*(u8*)(e + 0x22) != 0)
            return;
        e += 0x24;
    }

    n = data_ov004_020bf9f8;
    if (n > 0) {
        if (n > 0x14)
            limit = 0x14;
        else
            limit = n;
        step = 8;
        if (n > 0x14)
            step >>= 1;
        /* idx holds loop counter (r8); i holds display index (r4) */
        idx = 0;
        if (limit > 0) {
            e = &data_ov004_020bfa34;
            i = idx;
            do {
                int row = idx / 5;
                int col = idx % 5;
                int cx = col << 0x10;
                int rx = -(row << 0x10);
                int ret = -func_ov004_020b04c0();
                sp[1] = (ret - 0x10) << 0xc;
                sp[0] = 0x60000;
                {
                    int c2 = data_ov004_020bf9f8;
                    if (c2 < 5) {
                        sp[0] = (0x80 - ((c2 - 1) << 3)) << 0xc;
                    }
                }
                sp[0] = sp[0] + cx;
                sp[1] = sp[1] + rx;
                func_ov004_020b4b84(e, sp);
                *(u16*)(e + 0x20) = (u16)i;
                idx++;
                e += 0x24;
                i += step;
            } while (idx < limit);
        }
        data_ov004_020bfa04 = 0;
        return;
    }
    data_ov004_020bfa04 = 0x3c;
    data_ov004_020bfa20 = (void*)func_ov004_020b5368;
}
