#include "types.h"
extern u32 data_02099fd0;  /* RENDER_DMA_CHANNEL: 0x02099fd0 */
extern u32 data_020a60a4; /* saved bank: 0x020a60a4 */

extern void func_02059fa8(u32 dmaId);
extern void _ZN2GX22SetBankForSubBGExtPlttEt(u32 bank);

void _ZN3GXS16EndLoadBGExtPlttEv(void) {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        func_02059fa8(dmaId);
    }
    _ZN2GX22SetBankForSubBGExtPlttEt(data_020a60a4);
    data_020a60a4 = 0;
}
