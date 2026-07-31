//cpp
// @symbol _ZN7Message7DisplayEj
/* recovered: named members + shared header, real C++ method */
#include "Message.h"
typedef unsigned char u8;
typedef unsigned short u16;

extern "C" {
extern u8 data_0209d660;
extern short data_0209d6d4;
extern u8 data_0209d6c4;
extern u8 data_0209d668;
extern u8 data_0209d6a8;
void func_0201eaac(void);
int func_02054d88(void);
void MultiStore_Int(int a, int b, int n);
int func_02054fb0(void);
void MultiStore16(int a, int b, int n);
void _ZN7Message11DisplayTextEt(void* self);
int _ZN3G2S13GetBG0CharPtrEv(void);
int _ZN3G2S12GetBG0ScrPtrEv(void);
void Message_DrawCenteredLine(int a, int b);
}

void Message::Display(unsigned int msg)
{
    volatile int li;
    volatile unsigned short ls;
    volatile int li2;
    volatile unsigned short ls2;
    int p, s;
    data_0209d660 = 0;
    func_0201eaac();
    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);
    s = func_02054fb0();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);
    _ZN7Message11DisplayTextEt(((void*)this));
    data_0209d6d4 = 0x276;
    data_0209d6c4 = 0;
    data_0209d668 = 1;
    data_0209d6a8 = 0;
    p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
    li2 = 0;
    MultiStore_Int(li2, p, 0x2000);
    s = _ZN3G2S12GetBG0ScrPtrEv();
    ls2 = 0x2ff;
    MultiStore16(ls2, s, 0x800);
    Message_DrawCenteredLine(0xa0, 0x20);
    data_0209d6d4 = 0x279;
    Message_DrawCenteredLine(0x140, 0x20);
    data_0209d6d4 = 0x28b;
    Message_DrawCenteredLine(0x1e0, 0x20);
    data_0209d6d4 = 0x28f;
    Message_DrawCenteredLine(0x280, 0x20);
}
