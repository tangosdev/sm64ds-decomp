//cpp
extern "C" {
extern short data_0209fce8;
extern unsigned char data_0209fc9c;
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
    int zero = 0;
    data_0209fce8 = c;
    data_0209fc9c = zero;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = zero;
    if (d >= 0) data_0209fc84 = d;
    switch (data_0209fce8) {
    case 0x13:
    case 0x15:
        p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
        li0 = 0;
        MultiStore_Int(li0, p, 0x3000);
        s = _ZN3G2S12GetBG0ScrPtrEv();
        ls0 = 0x37f;
        MultiStore16(ls0, s, 0x800);
        break;
    default:
        p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
        li1 = 0;
        MultiStore_Int(li1, p, 0x2000);
        s = _ZN3G2S12GetBG0ScrPtrEv();
        ls1 = 0x2ff;
        MultiStore16(ls1, s, 0x800);
        break;
    }
    func_02033e50(a, b);
}
}
