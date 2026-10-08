#include "types.h"
extern int data_020a4c5c[];
extern int data_020a4c4c[];
extern u16 data_020a4c48[];

void func_0204f82c(void *a, int b, int c);
void func_0204f86c(void *a, int b, int c);

void func_0204921c(void *a, int mode)
{
    int i;
    int id;
    char *p;

    if (a == 0)
        return;
    p = *(char **)a;
    if (p == 0)
        return;

    id = *(u16 *)(p + 0x38);

    if (mode == 0) {
        if (id != 0x3b)
            return;
        data_020a4c5c[0] = 0x7f000;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 3;
        data_020a4c5c[1] = 0;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 1) {
        if (id != 0x3b)
            return;
        data_020a4c5c[0] = 0;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 3;
        data_020a4c5c[1] = 0;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 2) {
        if (id != 0x3b)
            return;
        data_020a4c5c[0] = 0x7f000;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 3;
        data_020a4c5c[1] = 0x7f000;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 3) {
        if (id != 0x3d)
            return;
        data_020a4c5c[0] = 0x7f000;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 0x70;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0;
    } else if (mode == 4) {
        if (id != 0x3d)
            return;
        data_020a4c5c[0] = 0;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 0x70;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0;
    } else if (mode == 5) {
        if (id != 0x3e)
            return;
        data_020a4c5c[0] = 0;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 8;
        data_020a4c5c[1] = 0x7f000;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0x100;
    } else {
        if (mode != 6)
            return;
        if (id != 0x3e)
            return;
        data_020a4c5c[0] = 0x7f000;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 8;
        data_020a4c5c[1] = 0;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0x100;
    }

    for (i = 0; i < 2; i++) {
        if (data_020a4c5c[i] < 0)
            data_020a4c5c[i] = 0;
        else if (data_020a4c5c[i] > 0x7f000)
            data_020a4c5c[i] = 0x7f000;
        func_0204f82c(a, data_020a4c48[i], data_020a4c5c[i] >> 12);
        if (data_020a4c5c[i] == 0)
            func_0204f86c(a, data_020a4c48[i], 1);
    }
}
