#include "private/arm9_wbt.h"

int WBT_MpParentSendHook(void *sendbuf, int send_size) {
    unsigned short r = 0;
    if (send_size > 0 && sendbuf != 0) {
        r = func_02065538(sendbuf, send_size);
    }
    return r;
}
