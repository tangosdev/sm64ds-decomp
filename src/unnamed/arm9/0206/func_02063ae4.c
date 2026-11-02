#include "types.h"
#include "private/arm9_wbt.h"
extern int func_02065a74(void);
extern int func_0206491c(unsigned int a, int b);
extern int func_02064888(unsigned int a, int b);
extern int func_02064c2c(int a, int b, int c, int d, int e, int f, int g);
extern int func_02065bc0(void);

int func_02063ae4(WBTWork *work, int idx, int r6) {
    WBTPeerRecord *e = &work->peer[idx];
    int r7 = 0;
    if (e->state == 2) {
        int r5;
        if (func_02065a74() == 0) r5 = 0xffff; else r5 = 1;
        if (e->command == 6) {
            int r8 = func_0206491c(e->block_id, e->seq_no);
            r7 = func_02064888(e->block_id, r8);
            if (r7 == 0) {
                r7 = func_02064c2c(r6, r5, e->cmd_counter, e->block_id, e->seq_no, 0, 0);
            } else {
                int t = func_02065bc0();
                r7 = func_02064c2c(r6, r5, e->cmd_counter, e->block_id, r8, r7, t);
            }
            e->state = 1;
        }
    }
    return r7;
}
