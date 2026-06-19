//cpp
extern "C" {
extern short data_0209d6d4;
extern unsigned char data_0209d6c0;
extern unsigned char data_0209d6a8;
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore_Int(int a, int b, int n);
extern void MultiStore16(int a, int b, int n);
extern int func_0201cfb8(unsigned short n);
extern void func_0201d418(int a, int b);
extern void func_0201d6a0(int a, int b);

void _ZN7Message22DisplayOptionsMenuTextEt(unsigned short n) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);
    s = _ZN3G2S12GetBG0ScrPtrEv();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);
    func_0201cfb8(n);
    data_0209d6d4 = 0x287;
    data_0209d6c0 = 1;
    func_0201d418(0x18f, 0x28);
    data_0209d6d4 = data_0209d6d4 + 1;
    func_0201d418(0x197, 0x28);
    data_0209d6a8 = data_0209d6a8 << 1;
    data_0209d6d4 = 0x283;
    func_0201d6a0(0x280, 0x20);
}
}
