#include "types.h"
#include "private/arm9_wbt.h"

extern WBTBlockInfoList *func_02065a94(int key);
extern int func_0206491c(unsigned int key, int val);
extern char *func_02064888(unsigned int id, int idx);
extern int func_02064d6c(u8 *dst, int a, u8 b, int c, int d, const void *p, int n);
extern int func_02065bc0(void);
extern void func_02064554(void *base, int a, int b, s16 c);

/* NitroSDK WBT: answer a peer's completed GET_BLOCK request (REQ_GET_BLOCK)
 * with one packet of the block. A COMMON block's packet comes straight from
 * the registered data (func_02064888); a USER block's comes from the
 * application, through a PREPARE_SEND_DATA (0xd) system callback on
 * system_cmd. Views are private/arm9_wbt.h's. */
int func_02063bc4(WBTWork *work, int idx, void *buf, int size) {
    int r6 = 0;
    WBTCommand *sys = &work->system_cmd;
    WBTPeerRecord *e = &work->peer[idx];
    if (e->state == 2) {
        int r7;
        if (WBT_GetOwnAid() == 0)
            r7 = 0xfffe;
        else
            r7 = 1;
        if (e->command == WBT_CMD_REQ_GET_BLOCK) {
            WBTBlockInfoList *node = func_02065a94(e->block_id);
            if (node != 0) {
                int v = func_0206491c(e->block_id, e->seq_no);
                if (node->block_type == WBT_BLOCK_LIST_TYPE_COMMON) {
                    char *data = func_02064888(e->block_id, v);
                    if (data == 0) {
                        r6 = func_02064d6c(buf, r7, e->cmd_counter, e->block_id, e->seq_no, 0, 0);
                    } else {
                        r6 = func_02064d6c(buf, r7, e->cmd_counter, e->block_id, v, data,
                                           func_02065bc0());
                    }
                } else if (node->block_type == WBT_BLOCK_LIST_TYPE_USER) {
                    sys->u.prepare_send_data.block_id = e->block_id;
                    sys->u.prepare_send_data.block_seq_no = v;
                    sys->u.prepare_send_data.own_packet_size = func_02065bc0();
                    sys->u.prepare_send_data.data_ptr = 0;
                    func_02064554(work, WBT_CMD_PREPARE_SEND_DATA, idx, 0);
                    if (sys->u.prepare_send_data.data_ptr != 0) {
                        r6 = func_02064d6c(buf, r7, e->cmd_counter,
                                           sys->u.prepare_send_data.block_id,
                                           sys->u.prepare_send_data.block_seq_no,
                                           sys->u.prepare_send_data.data_ptr,
                                           sys->u.prepare_send_data.own_packet_size);
                    }
                }
            }
            e->state = 1;
        }
    }
    return r6;
}
