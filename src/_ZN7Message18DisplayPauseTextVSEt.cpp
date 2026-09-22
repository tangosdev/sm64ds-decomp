//cpp
// @symbol _ZN7Message18DisplayPauseTextVSEt
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Message.h"
extern "C" {
extern short data_0209fce8;
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore_Int(int a, int b, int n);
extern void MultiStore16(int a, int b, int n);

}

void Message::DisplayPauseTextVS(unsigned short n)
{
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209fce8 = n;
    data_0209fc78 = 0;
    data_0209fc94 = 1;
    p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);
    s = _ZN3G2S12GetBG0ScrPtrEv();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);
    func_020341a8(0xc0, 0x20);
    data_0209fce8 = data_0209fce8 + 1;
    func_020341a8(0x160, 0x20);
    data_0209fce8 = 0x22;
    func_020341a8(0x200, 0x20);
}
