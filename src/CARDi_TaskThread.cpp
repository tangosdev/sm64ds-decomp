//cpp
/* NitroSDK CARDi_TaskThread (card_request.c): the card thread's body.
 * CARDi_InitCommon creates the thread on it. It sleeps until CARDi_SetTask
 * raises CARD_STAT_TASK, then runs the task. The task ends with
 * CARDi_EndTask, which clears the bit again.
 */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/card/card_common.h"



void CARDi_TaskThread(void *arg) {
    CARDiCommon *const p = &cardi_common;
    for (;;) {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        for (;;) {
            if (p->flag & CARD_STAT_TASK)
                break;
            p->cur_th = &p->thread;
            OS_SleepThread(0);
        }
        OS_RestoreInterrupts(bak_psr);
        (*p->task_func)(p);
    }
}
