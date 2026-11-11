//cpp
/* NitroSDK CARDi_OnReadCard (card_rom.c): the card-data IRQ handler for a
 * DMA read. Advance a page; at the end stop the DMA (MI_StopDma), take the
 * card IRQ (IE bit 19) off and end the read, else start the next page. */
#include "nitro/os/interrupt.h"
#include "nitro/card/card_rom.h"

extern "C" {

extern void MI_StopDma(u32 dmaNo);
extern void CARDi_ReadEnd(void);
extern void CARDi_SetCardDma(void);

void CARDi_OnReadCard(void) {
    cardi_common.src += CARD_ROM_PAGE_SIZE;
    cardi_common.dst += CARD_ROM_PAGE_SIZE;
    cardi_common.len -= CARD_ROM_PAGE_SIZE;
    if (cardi_common.len == 0) {
        MI_StopDma(cardi_common.dma);
        OS_DisableIrqMask(OS_IE_CARD_DATA);
        OS_ResetRequestIrqMask(OS_IE_CARD_DATA);
        CARDi_ReadEnd();
    } else {
        CARDi_SetCardDma();
    }
}
}
