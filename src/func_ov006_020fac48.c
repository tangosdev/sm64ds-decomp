// NONMATCHING: register allocation (div=21). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void func_ov006_020fa924(char* c);
extern unsigned char TOP_SCREEN_RELATED;

void func_ov006_020fac48(char* c, int idx)
{
    int n;
    if (*(int*)(c + 0x5000 + 0xc10) == 2) return;
    n = idx * 0x14;
    if (*(unsigned short*)(c + 0x5c0c + n) != 0) {
        *(unsigned short*)(c + 0x5c0c + n) = *(unsigned short*)(c + 0x5c0c + n) - 1;
        return;
    }
    TOP_SCREEN_RELATED = TOP_SCREEN_RELATED | 4;
    *(int*)(c + 0x5bfc + n) = 0x100000;
    *(int*)(c + 0x5c00 + n) = 0x40000;
    *(int*)(c + n + 0x5000 + 0xc04) = -0x1000;
    *(int*)(c + n + 0x5000 + 0xc08) = -0x1000;
    {
        int a = *(int*)(c + 0x5c00 + n);
        int b = *(int*)(c + 0x5bfc + n);
        *(unsigned char*)(c + n + 0x5000 + 0xc0f) = 1;
        *(volatile int*)0x4000018 =
            (0x1ff & (b >> 12)) | (0x1ff0000 & ((a >> 12) << 16));
    }
    func_ov006_020fa924(c);
}
