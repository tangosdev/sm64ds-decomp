#include "nitro/card/card_common.h"

/* NitroSDK CARD_GetCurrentBackupType: word 0 of the selected backup spec
 * (data_020a8760; see CARD_GetBackupTotalSize). */
extern int *data_020a8760;
int CARD_GetCurrentBackupType(void) {
    return data_020a8760[0];
}
