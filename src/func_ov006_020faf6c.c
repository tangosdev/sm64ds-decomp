typedef unsigned char u8;

#pragma opt_common_subs off

extern int _ZN4cstd4sqrtEy(unsigned long long val);
extern void func_ov006_020fb8fc(char *c, int a2, int a3, int a4, int a5, int a6);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);

void func_ov006_020faf6c(char *c, int idx)
{
    int i;
    int j;
    char *p;
    char *q;
    int dx;
    int dy;
    u8 st;
    int n;

    if (*(u8 *)(c + idx * 0x1c + 0x5bc8) == 0)
        return;

    p = c;
    for (i = 0; i < 0x30; i++, p += 0x38) {
        if (*(u8 *)(p + 0x4f0c) == 0)
            continue;
        if (*(u8 *)(p + 0x4f0d) < 2)
            continue;
        dx = (*(int *)(c + idx * 0x1c + 0x5bb0) - *(int *)(p + 0x4ed8)) >> 12;
        dy = (*(int *)(c + idx * 0x1c + 0x5bb4) - *(int *)(p + 0x4edc)) >> 12;
        if (_ZN4cstd4sqrtEy(dx * dx + dy * dy) > 0x18)
            continue;

        n = idx * 0x1c;
        *(short *)(c + 0x5c2a) = 0x20;
        *(u8 *)(c + i * 0x38 + 0x4f0c) = 0;
        *(u8 *)(c + i * 0x38 + 0x4f0e) = 0;
        func_ov006_020fb8fc(c,
                            *(int *)(c + n + 0x5bb0),
                            *(int *)(c + n + 0x5bb4),
                            2, 0, 0);
        q = c;
        for (j = 0; j < 0x1e; j++, q += 0x38) {
            if (*(u8 *)(q + 0x468c) == 0)
                continue;
            st = *(u8 *)(q + 0x468f);
            if (st == 0)
                continue;
            if (st < 5) {
                *(u8 *)(q + 0x468f) = 6;
                *(short *)(q + 0x4688) = 0x10;
            }
        }
        *(u8 *)(c + n + 0x5bc7) = 0;
        *(u8 *)(c + n + 0x5bc6) = 0;
        *(u8 *)(c + 0x5c34) = 0;
        _ZN5Sound12PlayBank2_2DEj(0x18a);
        return;
    }
}
