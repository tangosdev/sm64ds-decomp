#include "types.h"
extern u32 data_02099fd0;  /* RENDER_DMA_CHANNEL: 0x02099fd0 */
extern u32 data_020a60b4; /* 0x020a60b4 */
extern u32 data_020a60b0; /* 0x020a60b0 */

extern void func_02059fa8(u32 dmaId);
extern void _ZN2GX17SetBankForTexPlttEt(u32 bank);

void _ZN2GX14EndLoadTexPlttEv(void) {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        func_02059fa8(dmaId);
    }
    _ZN2GX17SetBankForTexPlttEt(data_020a60b4);
    data_020a60b4 = 0;
    data_020a60b0 = 0;
}
