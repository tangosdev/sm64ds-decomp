#include "types.h"
extern void _ZN3IRQ11DisableIRQsEj(u32 mask);

extern volatile u16 gCtrl1;    /* 0x020a644c */
extern u32 gData2[2];          /* 0x020a6450 */

void func_02059cb4(void) {
    if (gCtrl1 != 0)
        return;
    u32 *data = gData2;
    gCtrl1 = 1;
    data[0] = 0;
    data[1] = 0;
    _ZN3IRQ11DisableIRQsEj(4);
}
