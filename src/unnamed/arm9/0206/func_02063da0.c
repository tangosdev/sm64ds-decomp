#include "private/arm9_wbt.h"

extern int func_02064b2c(unsigned char *dst, unsigned short b, int c, int d);

/* NitroSDK WBT: when peer[idx]'s parsed record is a complete
 * REQ_GET_BLOCK_DONE, write the RES_GET_BLOCK_DONE reply (func_02064b2c) for that peer's bit, command counter and block id,
 * and mark the record consumed. */
int func_02063da0(WBTWork *work, int idx, void *buf, int size) {
    WBTPeerRecord *e;
    unsigned short bit;
    int r;
    e = &work->peer[idx];
    bit = (unsigned short)(1 << idx);
    r = 0;
    if (e->state == 2) {
        if (e->command == WBT_CMD_REQ_GET_BLOCK_DONE) {
            r = func_02064b2c(buf, bit, e->cmd_counter, e->block_id);
            e->state = 1;
        }
    }
    return r;
}
