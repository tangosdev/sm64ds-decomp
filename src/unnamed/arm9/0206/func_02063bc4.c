#include "types.h"
#include "private/arm9_wbt.h"

/* NitroSDK WBTBlockInfoList, the field read here (the type keeps the name
 * func_02065a94's definition gives it). */
typedef struct {
    char pad[0x32];
    /* 0x32 -- NitroSDK block_type: WBT_RegisterBlock (func_020658c0) sets
     * 0 (COMMON) when it is given the block's data and 1 (USER) when it is
     * not, so each packet has to be asked for with PREPARE_SEND_DATA */
    u16 block_type;
} Node65;

extern int func_02065a74(void);
extern Node65 *func_02065a94(int key);
extern int func_0206491c(unsigned int key, int val);
extern int func_02064888(int a, int b);
extern int func_02064d6c(int a, int b, int c, int d, int e, int f, int g);
extern int func_02065bc0(void);
extern void func_02064554(void *base, int a, int b, int c);

/* NitroSDK WBT: answer a peer's completed GET_BLOCK request (peer command 4)
 * with one packet of the block. A COMMON block's packet comes straight from
 * the registered data (func_02064888); a USER block's comes from the
 * application, through a PREPARE_SEND_DATA (0xd) system callback on
 * system_cmd. Views are private/arm9_wbt.h's. */
int func_02063bc4(void *base, int idx, int r8) {
    int r6 = 0;
    WBTWork *work = (WBTWork *)base;
    WBTCommand *sys = &work->system_cmd;
    WBTPeerRecord *e = &work->peer[idx];
    if (e->state == 2) {
        int r7;
        if (func_02065a74() == 0) r7 = 0xfffe; else r7 = 1;
        if (e->command == 4) {
            Node65 *node = func_02065a94(e->block_id);
            if (node != 0) {
                int v = func_0206491c(e->block_id, e->seq_no);
                if (node->block_type == 0) {
                    int r = func_02064888(e->block_id, v);
                    if (r == 0) {
                        r6 = func_02064d6c(r8, r7, e->cmd_counter, e->block_id, e->seq_no, 0, 0);
                    } else {
                        r6 = func_02064d6c(r8, r7, e->cmd_counter, e->block_id, v, r,
                                           func_02065bc0());
                    }
                } else if (node->block_type == 1) {
                    sys->u.prepare_send_data.block_id = e->block_id;
                    sys->u.prepare_send_data.block_seq_no = v;
                    sys->u.prepare_send_data.own_packet_size = func_02065bc0();
                    sys->u.prepare_send_data.data_ptr = 0;
                    func_02064554(base, 0xd, idx, 0);
                    if (sys->u.prepare_send_data.data_ptr != 0) {
                        r6 =
                            func_02064d6c(r8, r7, e->cmd_counter, sys->u.prepare_send_data.block_id,
                                          sys->u.prepare_send_data.block_seq_no,
                                          (int)sys->u.prepare_send_data.data_ptr,
                                          sys->u.prepare_send_data.own_packet_size);
                    }
                }
            }
            e->state = 1;
        }
    }
    return r6;
}
