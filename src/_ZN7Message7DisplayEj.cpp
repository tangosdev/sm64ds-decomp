//cpp
typedef unsigned char u8;
typedef unsigned short u16;

extern "C" {
extern u8 PLAYER_TALKING;
extern short CURR_MSG_ID;
extern u8 data_0209d6c4;
extern u8 data_0209d668;
extern u8 MSG_LINE_HEIGHT;
void func_0201eaac(void);
int func_02054d88(void);
void MultiStore_Int(int a, int b, int n);
int func_02054fb0(void);
void MultiStore16(int a, int b, int n);
void _ZN7Message11DisplayTextEt(void* self);
int _ZN3G2S13GetBG0CharPtrEv(void);
int _ZN3G2S12GetBG0ScrPtrEv(void);
void func_0201d6a0(int a, int b);
}

extern "C" void _ZN7Message7DisplayEj(void* self, unsigned int msg)
{
    volatile int li;
    volatile unsigned short ls;
    volatile int li2;
    volatile unsigned short ls2;
    int p, s;
    PLAYER_TALKING = 0;
    func_0201eaac();
    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);
    s = func_02054fb0();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);
    _ZN7Message11DisplayTextEt(self);
    CURR_MSG_ID = 0x276;
    data_0209d6c4 = 0;
    data_0209d668 = 1;
    MSG_LINE_HEIGHT = 0;
    p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
    li2 = 0;
    MultiStore_Int(li2, p, 0x2000);
    s = _ZN3G2S12GetBG0ScrPtrEv();
    ls2 = 0x2ff;
    MultiStore16(ls2, s, 0x800);
    func_0201d6a0(0xa0, 0x20);
    CURR_MSG_ID = 0x279;
    func_0201d6a0(0x140, 0x20);
    CURR_MSG_ID = 0x28b;
    func_0201d6a0(0x1e0, 0x20);
    CURR_MSG_ID = 0x28f;
    func_0201d6a0(0x280, 0x20);
}
