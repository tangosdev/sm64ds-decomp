#include "private/arm9_wbt.h"

/* No return statement when there is no room or no buffer: the ROM returns
 * from that path with r0 untouched, so a send_size <= 0 call gets the buffer
 * address back as its "length" (and a null buffer 0). Spelling that as
 * `return (int)sendbuf` would need a cast the SDK's void * buffer never
 * otherwise takes; WBT_MpParentSendHook returns 0 on the same path. */
int WBT_MpChildSendHook(void *sendbuf, int send_size) {
    if (send_size > 0 && sendbuf != 0)
        return func_020653cc(sendbuf, send_size);
}
