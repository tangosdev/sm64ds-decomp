#include "types.h"
extern void func_02059624(u32 bit);
extern void _ZN3IRQ11DisableIRQsEj(u32 mask);

extern volatile u16 data_020a6440;    /* 0x020a6440 */
extern u32 data_020a6444[2];          /* 0x020a6444 */

void func_02059bc0(void) {
    if (data_020a6440 != 0)
        return;
    data_020a6440 = 1;
    func_02059624(1);
    u32 *data = data_020a6444;
    data[0] = 0;
    data[1] = 0;
    _ZN3IRQ11DisableIRQsEj(0x10);
}
