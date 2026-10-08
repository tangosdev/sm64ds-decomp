#include "types.h"
extern void func_ov007_020b2614(int a, int b, int c);
extern int _ZN4cstd3modEii(int a, int b);
extern int func_ov007_020c1da0(int i);
extern void func_ov007_020aec94(int r4);

extern char *data_ov007_0210342c;

void func_ov007_020b1cf0(void)
{
    char *r6 = *(char **)(data_ov007_0210342c + 0xf4);
    char *r2 = *(char **)(data_ov007_0210342c + 8);
    int hi = *(s16 *)(r2 + 0xa);
    int r5 = *(int *)(r2 + 0xc);
    int flag = 0;

    if (hi == 8 && r5 == 0) {
        int anim;
        *(s16 *)(*(char **)(data_ov007_0210342c + 0xc) + 2) = 3;
        anim = (*(int *)(data_ov007_0210342c + 0xec) == 1) ? 0x3d : 0x14;
        func_ov007_020b2614(0, anim, 8);
    }

    if (r5 >= 0x28) {
        int m = _ZN4cstd3modEii(r5 - 0x28, 0x154);
        char *p;
        switch (m) {
        case 0:
            p = *(char **)(*(char **)(data_ov007_0210342c + 0xa8));
            *(int *)(p + 0x1c) = 0x3c;
            *(int *)(p + 0x18) = *(int *)(p + 0x1c);
            break;
        case 0x46:
            p = *(char **)(*(char **)(data_ov007_0210342c + 0x118));
            *(int *)(p + 0x1c) = 0x3c;
            *(int *)(p + 0x18) = *(int *)(p + 0x1c);
            break;
        case 0x8c:
            p = *(char **)(*(char **)(data_ov007_0210342c + 0x114));
            *(int *)(p + 0x1c) = 0x3c;
            *(int *)(p + 0x18) = *(int *)(p + 0x1c);
            break;
        case 0xd2:
            p = *(char **)(*(char **)(data_ov007_0210342c + 0x11c));
            *(int *)(p + 0x1c) = 0x3c;
            *(int *)(p + 0x18) = *(int *)(p + 0x1c);
            break;
        }
    }

    if (*(int *)(*(char **)(*(char **)(data_ov007_0210342c + 0x130)) + 0x10) == 0x1000
        && func_ov007_020c1da0(0) == 0) {
        char *r1 = *(char **)(data_ov007_0210342c + 0x50);
        if (*(u16 *)(r1 + 0xc) != 0
            && *(u16 *)(r1 + 0x14) == 0
            && (unsigned)*(int *)(r1 + 0x24) >= 1) {
            flag = 1;
            *(int *)(data_ov007_0210342c + 0x20) = 1;
            func_ov007_020b2614(1, 0, 8);
        } else {
            char *r1b = *(char **)(data_ov007_0210342c + 0x54);
            if (*(u16 *)r1b & 2) {
                if (!(*(u16 *)(r1b + 2) & 2)) {
                    *(int *)(data_ov007_0210342c + 0x20) = 0;
                    flag = 1;
                }
            }
        }
    }

    if (flag != 0) {
        *(s16 *)(*(char **)(data_ov007_0210342c + 8) + 2) = 4;
        return;
    }

    {
        char *r2b = *(char **)(r6 + 0x18);
        if (*(s16 *)(r2b + 0xc) == -1)
            return;
        if ((int)*(u16 *)(r2b + 0x10) != (int)*(u16 *)(r2b + 0x12) - 0x22)
            return;
        func_ov007_020aec94(8);
    }
}
