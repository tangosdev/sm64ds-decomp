// NONMATCHING: register allocation (div=35). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;

struct Elem {
    int f_0;            /* +0x50e8 within base */
    char _4[0x50f9-0x50e8-4];
    u8 active;          /* +0x50f9 */
};

extern int data_ov006_0212ef5c[];
extern int data_ov006_0212ef6c[];

int func_ov006_0211ba88(char *base, int idx)
{
    int n;
    int i;
    int limit;
    int *cur;
    u8 *flag;

    cur = (int *)(base + 0x50e8 + idx * 0x14);
    *cur = *cur - 0x10000;

    if ((*(int (***)(void))base)[0x8c / 4]() != 0) {
        limit = data_ov006_0212ef5c[*(u8 *)(base + 0x5000 + 0x627)];
    } else {
        limit = data_ov006_0212ef6c[*(u8 *)(base + 0x5000 + 0x627)];
    }

    flag = (u8 *)(base + 0x50f9 + idx * 0x14);
    if (*flag == 0) {
        n = idx;
        if (idx >= limit) {
            n = idx - limit;
            if (n < limit) {
                if (n == 0) {
                    *flag = 1;
                    return 0;
                }
            }
        }
        if (n > 0) {
            for (i = 0; i < n; i++) {
                int j = idx - i - 1;
                char *e = base + j * 0x14;
                if (*(u8 *)(e + 0x5000 + 0xf5) != 0) {
                    if ((*(int *)(base + 0x50e8 + idx * 0x14) - *(int *)(e + 0x5000 + 0xe8)) >> 0xc <= 4) {
                        *(u8 *)(base + 0x50f9 + idx * 0x14) = 1;
                        *(int *)(e + 0x5000 + 0xf4) = 4;
                    }
                }
            }
        }
    }

    if (*cur >> 0xc > -0x18) return 0;

    {
        char *e = base + idx * 0x14 + 0x5000;
        *(u8 *)(e + 0xf5) = 0;
        *(u8 *)(e + 0xf6) = 0;
    }
    return 0;
}
