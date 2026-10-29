#include "types.h"
#include "private/arm9_wbt.h"
extern void CpuCopy8(int a, int b, int c);

/* NitroSDK WBT (wireless block transfer): this raises a system callback for
 * the command a peer sent. `self` is the WBT work area; its system command
 * block (system_cmd, +0x138) is a WBTCommand, named here after the SDK's
 * fields, and `sel` is the peer's command type: 2 REQ_SYNC,
 * 8 REQ_GET_BLOCK_DONE, 10 REQ_USER_DATA, 12 SYSTEM_CALLBACK,
 * 13 PREPARE_SEND_DATA (always raised), 14 REQ_ERROR. The block's command
 * is 0xc, SYSTEM_CALLBACK.
 *
 * The 0x68-byte per-peer record (peer[], +0x1d4) holds what func_02063ea8
 * parsed out of that peer's packet; its fields are named after the callback
 * fields they feed. The views are in private/arm9_wbt.h. */
void func_02064554(int self, int sel, int idx, s16 a3)
{
    WBTWork *work = (WBTWork *)self;
    WBTPeerRecord *c = &work->peer[idx];
    WBTCommand *o = &work->system_cmd;

    if (sel != 0xd && c->cmd_counter == c->last_cmd_counter)
        return;

    o->command = 0xc;
    o->peer_bmp = (s16)(1 << idx);
    c->last_cmd_counter = c->cmd_counter;
    o->peer_cmd_counter = c->cmd_counter;
    o->result = a3;

    switch (sel) {
    case 8:
        o->u.blockdone.block_id = c->block_id;
        break;
    case 2:
        o->u.sync.num_of_list = c->num_of_list;
        o->u.sync.peer_packet_size = c->peer_packet_size;
        o->u.sync.my_packet_size = c->my_packet_size;
        break;
    case 10: {
        u8 v = c->user_data_size;
        if (v > 9) {
            o->u.user_data.size = 0;
            break;
        }
        o->u.user_data.size = v;
        CpuCopy8(c->user_data, (int)o->u.user_data.data, o->u.user_data.size);
    } break;
    case 12:
    case 14:
        break;
    }

    o->event = sel;
    {
        void (*fn)(char *) = o->callback;
        if (fn)
            fn((char *)o);
    }
}
