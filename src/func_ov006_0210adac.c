extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void func_02012790(int arg);
extern void func_ov004_020adb1c(int self);

#include "private/ov006_slotgrid.h"

void func_ov006_0210adac(char *c)
{
    unsigned char buf[3];
    int i;
    int total;
    int arg;
    char *self;

    if (DecIfAbove0_Byte((unsigned char *)(c + 0x503e)) != 0)
        return;

    self = c;
    for (i = 0; i < 3; i++) {
        int j;
        buf[i] = *(unsigned char *)(self + 0x5031 + i);
        j = 1;
        do {
            int rem = (i + (((int *)(self + 0x4fe4))[j] >> 12) / 80) % *(unsigned char *)(self + 0x503a);
            if (buf[i] != ((SlotGrid *)self)->sym[j][rem]) {
                buf[i] = 5;
                break;
            }
        } while (++j < 3);
        self = (char *)((unsigned)self | (unsigned)(i - i));
    }

    *(int *)(c + 0x5010) = -1;
    total = 0;
    {
        int k;
        for (k = 0; k < 3; k++) {
            if (buf[k] == *(unsigned char *)(c + 0x503b)) {
                if (k == 1) {
                    *(int *)(c + 0x5010) = k;
                    total += 6;
                } else if (k == 0 || k == 2) {
                    *(int *)(c + 0x5010) = k;
                    total += 3;
                }
                break;
            }
        }
    }

    arg = *(int *)(c + 0xb4);
    if (total > 0) {
        *(int *)(c + 0x5014) = total;
        *(int *)(c + 0x5000) = 3;
        func_02012790(0x26);
        *(unsigned char *)(c + 0x503e) = 0x28;
        arg += 1;
    } else {
        *(int *)(c + 0x5000) = 4;
        func_02012790(0xe);
        *(unsigned char *)(c + 0x503e) = 0x50;
    }
    func_ov004_020adb1c(arg);

    *(unsigned char *)(c + 0xc3) = 0;
}
