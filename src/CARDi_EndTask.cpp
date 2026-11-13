//cpp
#include "nitro/os/system.h"
#include "nitro/card/card_common.h"



/* NitroSDK CARDi_EndTask: take the callback, clear the busy and task bits,
 * wake every caller waiting on busy_q and, if the card thread is waiting
 * for work (CARD_STAT_RECV), the card thread itself; then, interrupts
 * restored, run the callback. */
void CARDi_EndTask(CARDiCommon *p) {
    MIDmaCallback callback = p->callback;
    void *arg = p->callback_arg;
    OSIntrMode mask = OS_DisableInterrupts();
    p->flag &= ~(CARD_STAT_BUSY | CARD_STAT_TASK);
    OS_WakeupThread(&p->busy_q);
    if (p->flag & CARD_STAT_RECV) {
        OS_WakeupThreadDirect(&p->thread);
    }
    OS_RestoreInterrupts(mask);
    if (callback != 0) {
        callback(arg);
    }
}
