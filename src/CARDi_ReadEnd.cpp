//cpp
/* NitroSDK CARDi_ReadEnd (card_rom.c): a ROM read is done. Mark the request
 * successful, then end it as CARDi_EndTask does: clear the busy and task
 * bits, wake the waiters and, if it is waiting (CARD_STAT_RECV), the card
 * thread, and run the callback. */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/card/card_common.h"

extern "C" {

void CARDi_ReadEnd(void) {
    CARDiCommon *const p = &cardi_common;
    MIDmaCallback callback = p->callback;
    void *arg = p->callback_arg;
    OSIntrMode mask;

    p->cmd->result = CARD_RESULT_SUCCESS;
    mask = OS_DisableInterrupts();
    p->flag &= ~(CARD_STAT_BUSY | CARD_STAT_TASK);
    OS_WakeupThread(&p->busy_q);
    if (p->flag & CARD_STAT_RECV) {
        OS_WakeupThreadDirect(&p->thread);
    }
    OS_RestoreInterrupts(mask);
    if (callback) {
        callback(arg);
    }
}
}
