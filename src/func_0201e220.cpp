//cpp
extern "C" {
extern short CURR_MSG_ID;
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore_Int(int a, int b, int n);
extern void MultiStore16(int a, int b, int n);
extern void func_0201d6a0(int a, int b);

void func_0201e220(int arg) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);
    s = _ZN3G2S12GetBG0ScrPtrEv();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);
    if (arg == 0) {
        func_0201d6a0(0x160, 0x20);
        CURR_MSG_ID = CURR_MSG_ID + 1;
        func_0201d6a0(0x200, 0x20);
        CURR_MSG_ID = CURR_MSG_ID + 1;
        func_0201d6a0(0xc0, 0x20);
    } else {
        func_0201d6a0(0xc0, 0x20);
        CURR_MSG_ID = CURR_MSG_ID + 1;
        func_0201d6a0(0x160, 0x20);
        CURR_MSG_ID = CURR_MSG_ID + 1;
        func_0201d6a0(0x200, 0x20);
    }
}
}
