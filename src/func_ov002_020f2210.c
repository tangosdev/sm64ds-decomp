#include "types.h"
#define LP(x) ((void*)(int)(x))
#define LI(x) ((int)((long long)(int)(x)))
extern int RandomIntInternal(int *seed);
extern int data_0209e650;

struct Obj {
    char pad[0xd4];
    unsigned char *d4;
};

#define A24 ((u16 *)(thiz->d4 + 0x24))
#define A30 ((u8 *)(thiz->d4 + 0x30))
#define A0C ((s16 *)(thiz->d4 + 0xc))
#define A1C ((s16 *)(thiz->d4 + 0x1c))
#define A14 ((u16 *)(thiz->d4 + 0x14))

void func_ov002_020f2210(struct Obj *thiz)
{
    int i;
    int base;
    u8 state;
    state = *(u8 *)(thiz->d4 + 0x34);
    if (state >= 3 && state > 5) return;
    i = 0;
    base = 0;
    for (; i < 3; i++) {
        u16 *p = (u16 *)(LI(thiz->d4) + LI(i) * 2);
        if (p[0x12] != 0) {
            *(volatile u16 *)LP(p + 0x12) -= 1;
            if (A24[i] == 0) {
                if (A30[i] == 0) {
                    unsigned int r = (unsigned int)RandomIntInternal(&data_0209e650);
                    if (r & 1) {
                        A0C[i] = -0x40;
                        A1C[i] = 1;
                    } else {
                        A0C[i] = 0x140;
                        A1C[i] = -1;
                    }
                    A14[i] = (u16)((r >> 16) % 48 + base);
                    A30[i] = 1;
                }
            }
        }
        base += 0x30;
    }
}
