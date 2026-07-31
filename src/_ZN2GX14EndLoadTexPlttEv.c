#include "types.h"
extern u32 GXi_DmaId;  /* RENDER_DMA_CHANNEL: 0x02099fd0 */
extern u32 GXi_SavedTexPlttBankB; /* 0x020a60b4 */
extern u32 GXi_SavedTexPlttBankA; /* 0x020a60b0 */

extern void func_02059fa8(u32 dmaId);
extern void _ZN2GX17SetBankForTexPlttEt(u32 bank);

void _ZN2GX14EndLoadTexPlttEv(void) {
    u32 dmaId = GXi_DmaId;
    if (dmaId != (u32)-1) {
        func_02059fa8(dmaId);
    }
    _ZN2GX17SetBankForTexPlttEt(GXi_SavedTexPlttBankB);
    GXi_SavedTexPlttBankB = 0;
    GXi_SavedTexPlttBankA = 0;
}
