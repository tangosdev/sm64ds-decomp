// NONMATCHING: different op / idiom (div=39). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;

extern void func_02012718(void *a, int b);
extern u8 data_020a0e40;

struct Row { u8 a, b, c, d; };
extern struct Row TOUCH_INPUT_ARR[];

void func_ov006_020e4ed4(char *this)
{
    int i = data_020a0e40;
    int ok = 0;
    char *s;
    if (TOUCH_INPUT_ARR[i].a != 0) {
        if (TOUCH_INPUT_ARR[i].b != 0) ok = 1;
    }
    if (ok == 0) return;

    s = this + 0x5000;
    {
        int x = *(int *)(s + 0x584);
        int y = *(int *)(s + 0x588);
        *(int *)(s + 0x594) = ((x >> 0xc) - TOUCH_INPUT_ARR[i].c) << 0xc;
        *(int *)(s + 0x598) = ((y >> 0xc) - TOUCH_INPUT_ARR[i].d) << 0xc;
    }
    *(u8 *)(s + 0x5b8) = 1;
    *(u16 *)(this + 0x5500 + 0xb2) = 0xc000;
    if (*(u8 *)(s + 0x5bd) == 0) {
        func_02012718((void *)0x1d2, *(int *)(s + 0x584));
        *(u8 *)((char *)(this + 0x5000) + 0x5bd) = 6;
    }
    s = this + 0x5000;
    *(int *)(s + 0x5a0) = 0;
    *(int *)(s + 0x5a4) = 0;
    *(int *)(s + 0x58c) = *(int *)(s + 0x584) + *(int *)(s + 0x594);
    *(int *)(s + 0x590) = *(int *)(s + 0x588) + *(int *)(s + 0x598);
    *(int *)(s + 0x5a8) = 0xff;
    *(u8 *)(s + 0x5be) = 0;
}
