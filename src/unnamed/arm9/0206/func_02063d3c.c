#include "private/arm9_wbt.h"

extern int func_02064a28(unsigned char *dst, unsigned short v, unsigned char b);

int func_02063d3c(WBTWork *work, int aid, void *buf, int size) {
    WBTPeerRecord *peer = &work->peer[aid];
    unsigned short mask = (unsigned short)(1 << aid);
    int ret = 0;
    if (peer->state == 2 && peer->command == WBT_CMD_REQ_USER_DATA) {
        ret = func_02064a28(buf, mask, peer->cmd_counter);
        peer->state = 1;
    }
    return ret;
}
