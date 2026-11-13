/* The backup identify task CARD_IdentifyBackup runs: put the backup type
 * (CARD_GetCurrentBackupType, the first word of the spec func_02060364
 * selected) in the request's type field, send CARD_REQ_IDENTIFY, then end
 * the task. */
#include "nitro/card/card_common.h"

void func_020607e8(CARDiCommon *p) {
    p->cmd->type = CARD_GetCurrentBackupType();
    _Z13CARDi_RequestP11CARDiCommonii(p, CARD_REQ_IDENTIFY, 1);
    _Z13CARDi_EndTaskP11CARDiCommon(p);
}
