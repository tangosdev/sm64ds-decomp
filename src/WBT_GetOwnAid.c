#include "nitro/wbt.h"

extern int data_0209a074;

/* NitroSDK WBT_SetOwnAid and WBT_GetOwnAid: this console's aid, 0 on the
 * parent (WBT_InitParent), -1 before a child has one (WBT_InitChild,
 * WBT_End). */
void WBT_SetOwnAid(int aid) {
    data_0209a074 = aid;
}

int WBT_GetOwnAid(void) {
    return data_0209a074;
}
