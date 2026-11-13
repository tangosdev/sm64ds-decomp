//cpp
/* NitroSDK CARD_backup.c CARD_IdentifyBackup, an older revision than pret's
 * (no OS_Terminate or CARD_CheckEnabled, no read-back request, and no
 * result): wait for the card to be free and claim it, select the backup
 * spec for the type (func_02060364), clear the callback, then run the
 * identify task func_020607e8 on this thread. */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/card/card_common.h"

extern "C" {


void func_02060364(int type);
void func_020607e8(CARDiCommon *p);
}


void CARD_IdentifyBackup(int type) {
    CARDiCommon *const p = &cardi_common;

    {
        OSIntrMode mask = OS_DisableInterrupts();
        while (p->flag & CARD_STAT_BUSY)
            OS_SleepThread(&p->busy_q);
        p->flag |= CARD_STAT_BUSY;
        OS_RestoreInterrupts(mask);
    }

    func_02060364(type);
    p->callback = 0;
    p->callback_arg = 0;
    cardi_common.cur_th = OSi_ThreadInfo.current;
    func_020607e8(p);
}
