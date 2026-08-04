// NONMATCHING: different op / idiom (div=35). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

void func_ov006_02106aa8(char *base, int idx)
{
    u8 *cnt = (u8 *)(base + 0x4fe9);
    char *p1 = base + idx * 2 + 0x4e00;

    *cnt = *cnt + 1;

    if (*(u16 *)(p1 + 0x78) != 0) {
        u16 *q = (u16 *)(base + 0x4e78 + idx * 2);
        *q = *q - 1;
        if (*(s16 *)(p1 + 0x78) < 0) {
            *(u16 *)(p1 + 0x78) = 0;
        }
    } else {
        u16 *r = (u16 *)(base + 0x4e30 + idx * 2);
        u8 flag = *(u8 *)(base + idx + 0x4000 + 0xf8a);
        int lim;
        *r = *r + 1;
        lim = (flag != 0) ? 4 : 8;
        if (*(u16 *)(p1 + 0x30) < (u16)lim) {
            return;
        }
        *(u16 *)(p1 + 0x30) = 0;
        {
            u16 *s = (u16 *)(base + 0x4de8 + idx * 2);
            *s = *s + 1;
        }
        if (*(u16 *)(base + idx * 2 + 0x4d00 + 0xe8) >= 5) {
            *(u16 *)(base + idx * 2 + 0x4d00 + 0xe8) = 0;
            *(u8 *)(base + idx + 0x4000 + 0xefa) = 0;
        }
    }
}
