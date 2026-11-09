//cpp
/* NitroSDK CARDi_TryReadCardDma (card_rom.c): a ROM read can go by DMA when
 * dst is word-aligned, the channel is a real one (0..3), the destination
 * misses the TCMs (CARDi_IsInTcm, inlined), and src and len are whole pages
 * and len is not 0. Set rom_stat's ROMCTRL value for a page read either way;
 * if DMA is possible, flush the destination, hook CARDi_OnReadCard on the
 * card IRQ (IE bit 19) and start the first page (CARDi_SetCardDma). Returns
 * whether the read went by DMA. */
#include "types.h"
#include "CP15.h"
#include "nitro/os/system.h"
#include "nitro/os/interrupt.h"
#include "nitro/card/card_rom.h"

extern "C" {

extern char func_01ff8000; /* the ITCM's end: its first address past 0x01ff8000 */
extern void CARDi_OnReadCard(void);
extern void CARDi_SetCardDma(void);

static inline int CARDi_IsInTcm(u32 buf, u32 len) {
    const u32 d = CP15::GetDTCMBaseAddress();
    return (buf + len > (u32)&func_01ff8000 && buf < 0x2000000) ||
           (d < buf + len && d + 0x4000 > buf);
}

int CARDi_TryReadCardDma(CARDRomStat *p) {
    CARDRomStat *stat;
    CARDiCommon *c;
    u32 len;
    int is_async;
    int whole_pages;
    int not_in_tcm;
    u32 dst;
    int dma_ok;
    OSIntrMode saved;

    stat = p;
    c = &cardi_common;
    is_async = 0;
    whole_pages = 0;
    not_in_tcm = 0;
    dma_ok = 0;

    dst = c->dst;

    if ((dst & 3) == 0) {
        if (c->dma <= 3)
            dma_ok = 1;
    }

    if (dma_ok != 0) {
        len = c->len;
        if (!CARDi_IsInTcm(dst, len))
            not_in_tcm = 1;
    }

    if (not_in_tcm != 0) {
        if (((c->src | c->len) & (CARD_ROM_PAGE_SIZE - 1)) == 0)
            whole_pages = 1;
    }

    if (whole_pages != 0) {
        if (c->len != 0)
            is_async = 1;
    }

    stat->ctrl = (*(u32 *)0x027ffe60 & ~0x7000000) | 0xa1000000;

    if (is_async != 0) {
        saved = OS_DisableInterrupts();
        CP15::FlushAndInvalidateDataCache(c->dst, c->len);
        CP15::InvalidateInstructionCache(c->dst, c->len);
        OS_SetIrqFunction(0x80000, CARDi_OnReadCard);
        OS_ResetRequestIrqMask(0x80000);
        OS_EnableIrqMask(0x80000);
        CP15::DrainWriteBuffer();
        OS_RestoreInterrupts(saved);
        CARDi_SetCardDma();
    }

    return is_async;
}
}
