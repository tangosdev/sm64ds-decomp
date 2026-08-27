#include "types.h"

extern s8 data_0209f310[];
extern s16 data_0209f358[];
extern u8 data_0209b2f0[];
extern u8 data_0209b2ec[];
extern u8 data_0209b2ed[];
extern u8 data_0209fc5c[];
extern u8 data_0209fc5d[];

void func_ov075_021165b0(char *self)
{
    int max;
    u8 b;
    int t;
    u8 *pb;
    int i;
    int n;
    u8 *pc;

    i = 0;
    pc = data_0209b2f0;
    do {
        i = i + 1;
        *pc = 0;
        pc = pc + 1;
    } while (i < 4);

    if (data_0209f310[0] != 0 || data_0209f310[1] != 0 || data_0209f310[2] != 0 ||
        data_0209f310[3] != 0 || data_0209f358[0] != 0 || data_0209f358[1] != 0 ||
        data_0209f358[2] != 0 || data_0209f358[3] != 0) {
        max = 0;
        i = max;
        pc = (u8 *)data_0209f310;
        do {
            t = (s8)*pc;
            i = i + 1;
            pc = pc + 1;
            if (max < t) {
                max = t & 0xff;
            }
        } while (i < 4);

        n = 0;
        pb = (u8 *)data_0209f310;
        pc = data_0209b2f0;
        i = n;
        do {
            t = (s8)*pb;
            i = i + 1;
            pb = pb + 1;
            if (t >= max) {
                *pc = 1;
                n = n + 1;
            } else {
                *pc = 0;
            }
            pc = pc + 1;
        } while (i < 4);

        if (n >= 2) {
            {
                int w;
                w = 0;
                pc = data_0209b2f0;
                i = w;
                do {
                    if (*pc != 0 && w < data_0209f358[i]) {
                        w = data_0209f358[i] & 0xff;
                    }
                    i = i + 1;
                    pc = pc + 1;
                } while (i < 4);

                pc = data_0209b2f0;
                i = 0;
                do {
                    if (*pc != 0 && data_0209f358[i] < w) {
                        *pc = 0;
                    }
                    i = i + 1;
                    pc = pc + 1;
                } while (i < 4);
            }
        }

        pc = data_0209b2f0;
        pb = data_0209b2ec;
        i = 0;
        do {
            if (*pc != 0 && *pb < 99) {
                *pb = *pb + 1;
            }
            i = i + 1;
            pc = pc + 1;
            pb = pb + 1;
        } while (i < 4);
    }

    b = data_0209b2ec[0];
    pc = data_0209fc5d;
    pb = data_0209b2ed;
    i = 1;
    do {
        if (*pc != 0) {
            if (*pb > b) {
                b = *pb;
            }
        }
        i = i + 1;
        pc = pc + 1;
        pb = pb + 1;
    } while (i < 4);

    pc = data_0209fc5c;
    pb = data_0209b2ec;
    i = 0;
    do {
        if (*pc == 0) {
            goto store0;
        }
        if (*pb < b) {
            goto store0;
        }
        if (b != 0) {
            *(u8 *)(self + i + 0x27c) = 1;
        } else {
        store0:
            *(u8 *)(self + i + 0x27c) = 0;
        }
        i = i + 1;
        pc = pc + 1;
        pb = pb + 1;
    } while (i < 4);
}
