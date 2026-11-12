//cpp
#include "nitro/mb/mb.h"

extern "C" {

/* data_020a0f10 is this console's player index on the link: the AID that
   func_0203ea5c takes from func_02040704, 0 for the parent. */
extern unsigned char data_020a0f10;

/* Nonzero unless this console is the host: a download-play child
   (MB_IsMultiBootChild) or any player but 0.
   func_0203d9b4 is the same test the other way round. */
int func_0203d974(void) {
    if (MB_IsMultiBootChild())
        return 1;
    return data_020a0f10 != 0;
}
}
