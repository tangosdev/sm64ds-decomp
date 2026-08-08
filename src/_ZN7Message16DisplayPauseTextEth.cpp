//cpp
// @symbol _ZN7Message16DisplayPauseTextEth
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Message.h"
extern "C" {
extern short data_0209d6d4;
extern unsigned char data_0209d6a8;
extern unsigned char data_0209d668;
/* func_0201d850 comes from decl_common.h. It used to be declared here as
   `int func_0201d850(unsigned char a)` and, in DisplayLevelClearText, as
   `int func_0201d850(signed char course)` -- one function, two signatures, neither
   matching the definition's `void func_0201d850(u32)`. */
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore_Int(int a, int b, int n);
extern void MultiStore16(int a, int b, int n);

}

void Message::DisplayPauseText(unsigned short n, unsigned char b)
{
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    func_0201d850(b);
    data_0209d6d4 = n;
    data_0209d6a8 = 0;
    data_0209d668 = 1;
    p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);
    s = _ZN3G2S12GetBG0ScrPtrEv();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);
    Message_DrawCenteredLine(0x80, 0x20);
    data_0209d6d4 = data_0209d6d4 + 1;
    Message_DrawCenteredLine(0x260, 0x20);
    data_0209d6d4 = data_0209d6d4 + 1;
    Message_DrawCenteredLine(0x120, 0x20);
    data_0209d6d4 = 0x28b;
    Message_DrawCenteredLine(0x1c0, 0x20);
}
