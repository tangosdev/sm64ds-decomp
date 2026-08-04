#include "types.h"
struct T {
    u8 pad[0x46c0];
    u8 grid[3][0x15];
    u8 idx[3];
    u8 pad2[6];
    u8 f708;
    u8 f709;
    u8 pad3;
    u8 count;
};

int func_ov006_0210c500(struct T *p)
{
    int ok;
    int first;
    int i;

    p->count = 0;
    ok = 1;
    for (i = 0; i < 3; i++) {
        if (p->grid[i][p->idx[i]] == 5) {
            p->count++;
        } else {
            first = p->grid[i][p->idx[i]];
            break;
        }
    }
    if (p->count >= 3) {
        first = 5;
    } else if (i < 2) {
        for (i = i + 1; i < 3; i++) {
            u8 c = p->grid[i][p->idx[i]];
            if (c == 5) {
                p->count++;
            } else if (first != c) {
                ok = 0;
            }
        }
    }
    if (ok) {
        if (p->f708 < 2) {
            u8 *q = (u8 *)(int)(((long long)(int)((u8 *)p + 0x4708)));
            *q = *q + 1;
        }
        p->f709 = first;
    }
    return ok;
}
