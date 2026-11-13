#include "private/arm9_wbt.h"

int func_02065bc0(void);
int func_02065bd0(void);
int func_02064eac(unsigned char *dst, unsigned short mask, unsigned char e2, short a, int b, int c);

int func_02063e08(WBTWork *work, int aid, void *buf, int size) {
    WBTPeerRecord *peer;
    unsigned short mask;
    int a, b, c, r;

    r = 0;
    peer = &work->peer[aid];
    mask = (1 << aid);
    if (peer->state == 2 && peer->command == WBT_CMD_REQ_SYNC) {
        a = WBT_NumOfRegisteredBlock();
        b = func_02065bc0();
        c = func_02065bd0();
        r = func_02064eac(buf, mask, peer->cmd_counter, a, b, c);
        peer->state = 1;
    }
    return r;
}
