//cpp
#include "types.h"
extern "C" {
extern s32 data_020a4c5c[];
extern s32 data_020a4c4c[];
extern u16 data_020a4c48[];
extern s32 data_020a4c54[];

void func_0204f86c(void *p, u16 idx, s32 val);
void func_0204f82c(void *p, u16 idx, s32 val);

void func_020490b0(void *c)
{
    s32 t[4];
    s32 i;
    s32 a;
    s32 e;
    s32 w;
    s32 flag;

    if (c == 0) {
        return;
    }
    if (*(s32 *)c == 0) {
        return;
    }

    t[0] = 0;
    t[1] = 0x7f000;
    t[2] = 0;
    t[3] = 0;

    for (i = 0; i < 2; i++) {
        flag = 0;
        a = data_020a4c4c[i];
        if (a > 0) {
            e = data_020a4c5c[i];
            if (e == 0) {
                flag = 1;
            }
            data_020a4c5c[i] = data_020a4c5c[i] + a;
            if (data_020a4c5c[i] >= data_020a4c54[i]) {
                data_020a4c5c[i] = data_020a4c54[i];
                data_020a4c4c[i] = t[0];
            }
        } else if (a < 0) {
            data_020a4c5c[i] = data_020a4c5c[i] + a;
            if (data_020a4c5c[i] <= data_020a4c54[i]) {
                data_020a4c5c[i] = data_020a4c54[i];
                data_020a4c4c[i] = 0;
            }
        } else {
            continue;
        }

        w = data_020a4c5c[i];
        if (w > 0x7f000) {
            data_020a4c5c[i] = t[1];
        } else if (w < 0) {
            data_020a4c5c[i] = t[2];
            func_0204f86c(c, data_020a4c48[i], 1);
        }
        if (flag != 0) {
            func_0204f86c(c, data_020a4c48[i], t[3]);
        }
        func_0204f82c(c, data_020a4c48[i], data_020a4c5c[i] >> 12);
    }
}

}
