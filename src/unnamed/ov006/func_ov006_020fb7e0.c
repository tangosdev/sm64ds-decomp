// NONMATCHING: different op / idiom (div=18). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;

extern void func_ov006_020fbb2c(char *c, int idx);

void func_ov006_020fb7e0(char *thiz)
{
    int i;
    char *b = thiz;
    for (i = 0; i < 0x1e; i++) {
        if (*(u8 *)(b + 0x5964) != 0) {
            u16 *cnt = (u16 *)(b + 0x5960);
            int lim;
            u8 sub;
            (*cnt)++;
            if (*cnt >= 6) {
                *(u16 *)(b + 0x5960) = 0;
                (*(u8 *)(b + 0x5965))++;
                lim = 3;
                if (*(u8 *)(b + 0x5967) == 2) lim = 5;
                sub = *(u8 *)(b + 0x5968);
                if (sub != 0) {
                    if (lim - 2 == *(u8 *)(b + 0x5965)) {
                        char *q = thiz + (sub - 1) * 0xc + 0x5000;
                        *(u8 *)(q + 0xbd4) = 0;
                        *(u8 *)(q + 0xbd6) = 1;
                    }
                }
                if (*(u8 *)(b + 0x5965) >= lim) {
                    *(u8 *)(b + 0x5966) = 0;
                    *(u8 *)(b + 0x5964) = 0;
                    if (*(u16 *)(b + 0x5962) != 0) {
                        func_ov006_020fbb2c(thiz, i);
                    }
                }
            }
        }
        b = b + 0x14;
    }
}
