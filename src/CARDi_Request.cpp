//cpp
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/os/system.h"
#include "nitro/card/card_common.h"
#include "nitro/pxi/fifo.h"

#define TRUE 1

/* The SDK's card_common.h: a request word to the ARM7's card handler. */
static inline void CARDi_SendPxi(u32 data) {
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_FS, data, TRUE) < 0)
        ;
}

/* NitroSDK CARDi_Request (card_request.c): the first time, wait for the
 * ARM7's card handler on PXI_FIFO_TAG_FS and send it CARD_REQ_INIT; then
 * flush the command block and send req_type (CARD_REQ_INIT also sends the
 * block's address) until the result is not CARD_RESULT_TIMEOUT or the
 * retries run out, sleeping until CARDi_OnFifoRecv clears CARD_STAT_REQ.
 * This SDK does not invalidate the block after the reply. */
int CARDi_Request(CARDiCommon *p, int req_type, int retry_count) {
    if ((p->flag & CARD_STAT_INIT_CMD) == 0) {
        p->flag |= CARD_STAT_INIT_CMD;
        while (!PXI_IsCallbackReady(PXI_FIFO_TAG_FS, PXI_PROC_ARM7)) {
            OS_SpinWait(100);
        }

        (void)CARDi_Request(p, CARD_REQ_INIT, 1);
    }

    DC_FlushRange(p->cmd, sizeof(*p->cmd));
    DC_WaitWriteBufferEmpty();

    do {
        p->command = req_type;
        p->flag |= CARD_STAT_REQ;
        CARDi_SendPxi((u32)req_type);

        switch (req_type) {
        case CARD_REQ_INIT:
            CARDi_SendPxi((u32)p->cmd);
            break;
        }
        {
            OSIntrMode bak_psr = OS_DisableInterrupts();
            while ((p->flag & CARD_STAT_REQ) != 0) {
                OS_SleepThread(0);
            }
            (void)OS_RestoreInterrupts(bak_psr);
        }
    } while ((p->cmd->result == CARD_RESULT_TIMEOUT) && (--retry_count > 0));

    return (p->cmd->result == CARD_RESULT_SUCCESS);
}
