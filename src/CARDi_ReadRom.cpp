//cpp
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/card/card_rom.h"

extern "C" {

void MI_StopDma(u32 dmaNo);
void CARDi_ReadRomSyncCore(CARDiCommon *c);

/* NitroSDK CARDi_ReadRom (card_rom.c): wait out the request in flight and
 * claim the card, record the transfer, then read by DMA if that can start
 * now, else run the read task (on the card thread if asynchronous). */
void CARDi_ReadRom(u32 dma, const void *src, void *dst, u32 len, MIDmaCallback callback, void *arg,
                   int is_async) {
    CARDiCommon *const c = &cardi_common;
    CARDRomStat *const p = &rom_stat;

    {
        OSIntrMode mask = OS_DisableInterrupts();
        while (c->flag & 4)
            OS_SleepThread(&c->busy_q);
        c->flag |= 4;
        OS_RestoreInterrupts(mask);
    }

    c->dma = dma;
    c->src = (u32)src;
    c->dst = (u32)dst;
    c->len = len;
    c->callback = callback;
    c->callback_arg = arg;

    if (dma <= 3)
        MI_StopDma(dma);

    if (CARDi_TryReadCardDma(p)) {
        if (!is_async)
            CARD_WaitRomAsync();
    } else if (is_async) {
        CARDi_SetTask(CARDi_ReadRomSyncCore);
    } else {
        c->cur_th = data_020a6134.current;
        CARDi_ReadRomSyncCore(c);
    }
}
}
