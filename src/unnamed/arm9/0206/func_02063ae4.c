#include "types.h"
#include "private/arm9_wbt.h"
extern int func_0206491c(unsigned int a, int b);
extern char *func_02064888(unsigned int id, int idx);
extern int func_02064c2c(u8 *dst, int a, int b, int c, int d, const void *p, int n);
extern int func_02065bc0(void);

int func_02063ae4(WBTWork *work, int idx, void *buf, int size) {
    WBTPeerRecord *e = &work->peer[idx];
    int r7 = 0;
    if (e->state == 2) {
        int r5;
        if (WBT_GetOwnAid() == 0)
            r5 = 0xffff;
        else
            r5 = 1;
        if (e->command == WBT_CMD_REQ_GET_BLOCKINFO) {
            int r8 = func_0206491c(e->block_id, e->seq_no);
            char *data = func_02064888(e->block_id, r8);
            if (data == 0) {
                r7 = func_02064c2c(buf, r5, e->cmd_counter, e->block_id, e->seq_no, 0, 0);
            } else {
                int t = func_02065bc0();
                r7 = func_02064c2c(buf, r5, e->cmd_counter, e->block_id, r8, data, t);
            }
            e->state = 1;
        }
    }
    return r7;
}
