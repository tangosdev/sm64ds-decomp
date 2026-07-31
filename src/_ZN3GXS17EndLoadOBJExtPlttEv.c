#include "types.h"
extern u32 GXi_DmaId;  /* RENDER_DMA_CHANNEL: 0x02099fd0 */
extern u32 GXi_SavedSubOBJExtPlttBank; /* saved bank: 0x020a60a8 */

extern void func_02059fa8(u32 dmaId);
extern void _ZN2GX23SetBankForSubOBJExtPlttEt(u32 bank);

void _ZN3GXS17EndLoadOBJExtPlttEv(void) {
    u32 dmaId = GXi_DmaId;
    if (dmaId != (u32)-1) {
        func_02059fa8(dmaId);
    }
    _ZN2GX23SetBankForSubOBJExtPlttEt(GXi_SavedSubOBJExtPlttBank);
    GXi_SavedSubOBJExtPlttBank = 0;
}
