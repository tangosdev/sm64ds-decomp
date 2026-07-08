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
extern void func_0201d6a0(int a, int b);
extern u8 PLAYER_TALKING;
extern u8 data_0209d668;
extern u8 MSG_LINE_HEIGHT;
extern s16 CURR_MSG_ID;
void func_0201cd08(int arg)
{
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    PLAYER_TALKING = 0;
    func_0201eaac();
    data_0209d668 = 1;
    MSG_LINE_HEIGHT = 0;
    p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);
    s = _ZN3G2S12GetBG0ScrPtrEv();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);
    CURR_MSG_ID = (s16)arg;
    func_0201d418(CURR_MSG_ID == 0x29a ? 0x105 : 0xc5, 0xb0);
    CURR_MSG_ID = 0x293;
    MSG_LINE_HEIGHT = MSG_LINE_HEIGHT << 1;
    func_0201d6a0(0x285, 0xa);
    CURR_MSG_ID = CURR_MSG_ID + 1;
    func_0201d6a0(0x291, 0xa);
}
}
