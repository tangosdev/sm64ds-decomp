// NONMATCHING: different op / idiom (div=28). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void SetSubBg0Offset(int a, int b);
extern void func_02012790(int a);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int a);
extern unsigned char BOTTOM_SCREEN_RELATED;
extern int data_ov006_0212ed00[];

void func_ov006_02104920(char *c, int idx)
{
    int n = idx * 0xc;
    int cnt;
    *(unsigned short *)(c + 0x4690 + n) = *(unsigned short *)(c + 0x4690 + n) + 1;
    cnt = *(unsigned short *)(c + 0x4690 + n);
    if ((unsigned int)cnt >= 0x20) {
        *(unsigned short *)(c + 0x4690 + n) = 0;
        *(unsigned char *)(c + n + 0x4000 + 0x693) = 0;
        SetSubBg0Offset(0, 0);
        BOTTOM_SCREEN_RELATED &= ~1;
        func_02012790(0x12f);
        *(unsigned char *)(c + 0x4fe2) -= 1;
        return;
    }
    *(int *)(c + 0x468c + n) = data_ov006_0212ed00[cnt >> 3];
    {
        char *q = c + n + 0x4600;
        unsigned short t = *(unsigned short *)(q + 0x90);
        if (t == 1 || t == 0x11)
            _ZN5Sound12PlayBank2_2DEj(0x1fc);
    }
    SetSubBg0Offset(0, *(int *)(c + 0x468c + n));
}
