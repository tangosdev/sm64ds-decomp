//cpp
extern "C" {
extern short CURR_MSG_ID;
extern unsigned char PLAYER_TALKING;
extern unsigned char data_0209d668;
extern unsigned char MSG_LINE_HEIGHT;
extern int func_0201eaac();
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore_Int(int a, int b, int n);
extern void MultiStore16(int a, int b, int n);
extern void func_0201d418(int a, int b);
extern void func_0201d6a0(int a, int b);

void _ZN7Message19DisplayDontSaveTextEt(unsigned short n) {
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
    CURR_MSG_ID = n;
    func_0201d418(0x105, 0xb0);
    CURR_MSG_ID = 0x293;
    MSG_LINE_HEIGHT = MSG_LINE_HEIGHT << 1;
    func_0201d6a0(0x285, 0xa);
    CURR_MSG_ID = CURR_MSG_ID + 1;
    func_0201d6a0(0x291, 0xa);
}
}
