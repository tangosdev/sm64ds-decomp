/* NitroSDK CARDi_SetCardDma (card_rom.c): start a card ROM read by DMA.
 * Point channel dma at the card data port with dst as the destination, send
 * the read command for ROM address src (CARDi_SetRomOp), then start the card
 * with rom_stat's ctrl. */
#include "nitro/card/card_rom.h"
#include "nitro/hw/registers.h"

extern void CARDi_SetRomOp(u32 cmd1, u32 cmd2);

static inline void CARDi_SetRomOpReadPage1(u32 src) {
    CARDi_SetRomOp((u32)(MROMOP_G_READ_PAGE | (src >> 8)), (u32)(src << 24));
}

/* One DMA channel's registers at 0x040000b0 + n * 12: source, destination,
 * control. */
typedef struct {
    u32 src;
    u32 dst;
    u32 ctrl;
} DmaCh;

void CARDi_SetCardDma(void) {
    CARDiCommon *const c = &cardi_common;
    CARDRomStat *const p = &rom_stat;
    DmaCh *ch = (DmaCh *)(0x040000b0 + c->dma * 12);
    ch->src = (u32)&REG_CARD_DATA;
    ch->dst = c->dst;
    ch->ctrl = 0xaf000001;
    CARDi_SetRomOpReadPage1(c->src);
    REG_CARD_CNT = p->ctrl;
}
