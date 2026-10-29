//cpp
#include "IRQ.h"

extern "C" {

extern unsigned short data_020a6430;
extern int data_020a6438;
extern int data_020a6434;
extern void func_02059700(void);
extern void func_02059624(int bit);

void func_02059788(void)
{
    if (data_020a6430 != 0)
        return;
    data_020a6430 = 1;
    func_02059624(0);
    (&data_020a6438)[0] = 0;
    (&data_020a6438)[1] = 0;
    *(volatile unsigned short *)0x4000102 = 0;
    *(volatile unsigned short *)0x4000100 = 0;
    *(volatile unsigned short *)0x4000102 = 0xc1;
    IRQ::SetIRQHandler(8, func_02059700);
    IRQ::EnableIRQs(8);
    data_020a6434 = 0;
}
}
