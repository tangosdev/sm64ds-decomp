#include "private/arm9_wbt.h"

extern int func_02064b2c(int off, unsigned short b, int c, int d);

/* NitroSDK WBT: when peer[idx]'s parsed record is a complete
 * REQ_GET_BLOCK_DONE (command 8), write the RES_GET_BLOCK_DONE reply
 * (func_02064b2c, tag 9) for that peer's bit, command counter and block id,
 * and mark the record consumed. */
int func_02063da0(char *base, int idx, int c) {
    WBTPeerRecord *e;
    unsigned short bit;
    int r;
    e = &((WBTWork *)base)->peer[idx];
    bit = (unsigned short)(1 << idx);
    r = 0;
    if (e->state == 2) {
        if (e->command == 8) {
            r = func_02064b2c(c, bit, e->cmd_counter, e->block_id);
            e->state = 1;
        }
    }
    return r;
}
