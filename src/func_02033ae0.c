typedef unsigned char u8;
typedef short s16;
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore_Int(int a, int b, int n);
extern void MultiStore16(int a, int b, int n);
extern void func_02033a80(int a);
extern void func_02033e50(int a, int b);
extern void func_020341a8(int a, int b);
extern s16 data_0209fce8;
extern u8 data_0209fcb0;
extern u8 data_0209fc78;

void func_02033ae0(int arg)
{
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);
    s = _ZN3G2S12GetBG0ScrPtrEv();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);
    func_02033a80(arg);
    data_0209fce8 = 0xc;
    data_0209fcb0 = 1;
    func_02033e50(0x18f, 0x28);
    data_0209fce8 = data_0209fce8 + 1;
    func_02033e50(0x197, 0x28);
    {
        u8 old = *(volatile u8 *)&data_0209fc78;
        data_0209fce8 = 8;
        data_0209fc78 = old << 1;
    }
    func_020341a8(0x280, 0x20);
}
