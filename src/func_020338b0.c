//cpp
// NONMATCHING: different op / idiom (div=19). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
extern short data_0209fce8;
extern unsigned char CONNECTION_ERROR;
extern unsigned char data_0209fc78;
extern unsigned char data_0209fc84;
extern unsigned char data_0209fc94;
extern void func_02034504(void);
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore_Int(int a, int b, int n);
extern void MultiStore16(int a, int b, int n);
extern void func_02033e50(int a, int b);

void func_020338b0(int a, int b, short c, int d) {
    int p, s;
    volatile int li0, li1;
    volatile unsigned short ls0, ls1;
    data_0209fce8 = c;
    CONNECTION_ERROR = 0;
    func_02034504();
    data_0209fc78 = 0;
    if (d >= 0) data_0209fc84 = d;
    data_0209fc94 = 1;
    if (data_0209fce8 == 0x13) {
        li0 = 0;
        p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
        MultiStore_Int(li0, p, 0x3000);
        s = _ZN3G2S12GetBG0ScrPtrEv();
        ls0 = 0x37f;
        MultiStore16(ls0, s, 0x800);
    } else if (data_0209fce8 == 0x15) {
        li1 = 0;
        p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
        MultiStore_Int(li1, p, 0x2000);
        s = _ZN3G2S12GetBG0ScrPtrEv();
        ls1 = 0x2ff;
        MultiStore16(ls1, s, 0x800);
    }
    func_02033e50(a, b);
}
}
