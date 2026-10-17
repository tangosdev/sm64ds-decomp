//cpp
#include "IRQ.h"

extern "C" {
extern volatile u16 data_020a644c;    /* 0x020a644c */
extern u32 data_020a6450[2];          /* 0x020a6450 */

void func_02059cb4(void) {
    if (data_020a644c != 0)
        return;
    u32 *data = data_020a6450;
    data_020a644c = 1;
    data[0] = 0;
    data[1] = 0;
    IRQ::DisableIRQs(4);
}
}
