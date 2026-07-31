//cpp
extern "C" {
typedef unsigned char u8;
typedef short s16;
extern void func_0201eaac(void);
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore_Int(int a, int b, int n);
extern void MultiStore16(int a, int b, int n);
extern void func_0201d418(int a, int b);
extern void Message_DrawCenteredLine(int a, int b);
extern u8 data_0209d660;
extern u8 data_0209d668;
extern u8 data_0209d6a8;
extern s16 data_0209d6d4;
void func_0201cd08(int arg)
{
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209d660 = 0;
    func_0201eaac();
    data_0209d668 = 1;
    data_0209d6a8 = 0;
    p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);
    s = _ZN3G2S12GetBG0ScrPtrEv();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);
    data_0209d6d4 = (s16)arg;
    func_0201d418(data_0209d6d4 == 0x29a ? 0x105 : 0xc5, 0xb0);
    data_0209d6d4 = 0x293;
    data_0209d6a8 = data_0209d6a8 << 1;
    Message_DrawCenteredLine(0x285, 0xa);
    data_0209d6d4 = data_0209d6d4 + 1;
    Message_DrawCenteredLine(0x291, 0xa);
}
}
