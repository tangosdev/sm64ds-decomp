//cpp
/* The backup read request (NitroSDK card_backup.c; this older SDK has one
 * entry point per request where pret's has CARDi_RequestStreamCommand):
 * wait for the card to be free and claim it, record the transfer, then run
 * the read task func_0206071c, on the card thread if is_async. Synchronous
 * calls return whether the request succeeded. */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/card/card_common.h"

void func_0206071c(CARDiCommon *p);

int func_02060558(u32 src, void *dst, u32 len, MIDmaCallback callback, void *arg, int is_async) {
    CARDiCommon *const p = &cardi_common;

    {
        OSIntrMode mask = OS_DisableInterrupts();
        while (p->flag & CARD_STAT_BUSY)
            OS_SleepThread(&p->busy_q);
        p->flag |= CARD_STAT_BUSY;
        OS_RestoreInterrupts(mask);
    }

    p->src = src;
    p->dst = (u32)dst;
    p->len = len;
    p->callback = callback;
    p->callback_arg = arg;
    if (is_async) {
        CARDi_SetTask(func_0206071c);
        return 1;
    }
    cardi_common.cur_th = OSi_ThreadInfo.current;
    func_0206071c(p);
    return p->cmd->result == CARD_RESULT_SUCCESS;
}
