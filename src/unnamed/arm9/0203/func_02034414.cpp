//cpp
extern "C" {
extern short data_0209fce8;
extern unsigned char data_0209fc9c;
extern unsigned char data_0209fc94;
extern unsigned char data_0209fc78;
extern int func_02034504(void);
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore_Int(int a, int b, int n);
extern void MultiStore16(int a, int b, int n);
extern void func_02033e50(int a, int b);
extern void func_020341a8(int a, int b);

void func_02034414(unsigned short n) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);
    s = _ZN3G2S12GetBG0ScrPtrEv();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);
    data_0209fce8 = n;
    func_02033e50(0x105, 0xb0);
    data_0209fce8 = 0xf;
    data_0209fc78 = data_0209fc78 << 1;
    func_020341a8(0x285, 0xa);
    data_0209fce8 = data_0209fce8 + 1;
    func_020341a8(0x291, 0xa);
}
}
