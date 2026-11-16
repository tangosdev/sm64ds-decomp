//cpp
/* The backup write request (NitroSDK card_backup.c; this older SDK has one
 * entry point per request where pret's has CARDi_RequestStreamCommand):
 * write len bytes from RAM src to backup address dst. Wait for the card to
 * be free and claim it, record the transfer, then run the write task
 * func_0206062c, on the card thread if is_async. Synchronous calls return
 * whether the request succeeded. */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/card/card_common.h"

void func_0206062c(CARDiCommon *p);

extern "C" int func_02060484(u32 dst, const void *src, u32 len, MIDmaCallback callback, void *arg,
                             int is_async) {
    CARDiCommon *const p = &cardi_common;

    {
        OSIntrMode mask = OS_DisableInterrupts();
        while (p->flag & CARD_STAT_BUSY)
            OS_SleepThread(&p->busy_q);
        p->flag |= CARD_STAT_BUSY;
        OS_RestoreInterrupts(mask);
    }

    p->src = (u32)src;
    p->dst = dst;
    p->len = len;
    p->callback = callback;
    p->callback_arg = arg;
    if (is_async) {
        CARDi_SetTask(func_0206062c);
        return 1;
    }
    cardi_common.cur_th = OSi_ThreadInfo.current;
    func_0206062c(p);
    return p->cmd->result == CARD_RESULT_SUCCESS;
}
