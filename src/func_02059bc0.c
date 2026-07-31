#include "types.h"
extern void func_02059624(u32 bit);
extern void _ZN3IRQ11DisableIRQsEj(u32 mask);

extern volatile u16 gCtrl0;    /* 0x020a6440 */
extern u32 gData0[2];          /* 0x020a6444 */

void func_02059bc0(void) {
    if (gCtrl0 != 0)
        return;
    gCtrl0 = 1;
    func_02059624(1);
    u32 *data = gData0;
    data[0] = 0;
    data[1] = 0;
    _ZN3IRQ11DisableIRQsEj(0x10);
}
