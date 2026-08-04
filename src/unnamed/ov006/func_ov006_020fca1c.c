#include "types.h"
#pragma opt_common_subs off

extern void func_ov006_020fb8fc(char *c, int a2, int a3, int a4, int a5, int a6);
extern void func_ov006_020fc1b4(char *base, int val);
extern void func_02012718(void *a, int b);

void func_ov006_020fca1c(char *c, int idx)
{
    int i;
    char *p;
    int dx;
    int dy;
    int n;

    if (*(u8 *)(c + idx * 0x38 + 0x468f) == 5)
        return;

    p = c;
    for (i = 0; i < 4; i++, p += 0xc) {
        if (*(u8 *)(p + 0x5bd4) == 0)
            continue;

        dx = (*(int *)(p + 0x5bcc) - *(int *)(c + idx * 0x38 + 0x4660)) >> 12;
        dy = (*(int *)(p + 0x5bd0) - *(int *)(c + idx * 0x38 + 0x4664)) >> 12;
        if (dx < -0x10) continue;
        if (dx > 0x18) continue;
        if (dy < -0x10) continue;
        if (dy > 0x18) continue;

        n = idx * 0x38;
        *(u8 *)(c + n + 0x468c) = 0;
        *(u8 *)(c + n + 0x468d) = 0;
        func_ov006_020fb8fc(c,
                            *(int *)(c + i * 0xc + 0x5bcc),
                            *(int *)(c + i * 0xc + 0x5bd0),
                            2, 0, i + 1);
        func_ov006_020fc1b4(c, 0);
        func_02012718((void *)0x18c, *(int *)(c + n + 0x4000 + 0x660));
        return;
    }
}
