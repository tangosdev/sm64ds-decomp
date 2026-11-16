#include "types.h"
#include "private/arm9_wbt.h"
#include "nitro/mi/memory.h"

/* NitroSDK WBT (wireless block transfer): this raises a system callback for
 * the command a peer sent. `work` is the WBT work area; its system command
 * block (system_cmd, +0x138) is a WBTCommand, named here after the SDK's
 * fields, and `sel` is the peer's command type (PREPARE_SEND_DATA is always
 * raised). The block's command is SYSTEM_CALLBACK.
 *
 * The 0x68-byte per-peer record (peer[], +0x1d4) holds what func_02063ea8
 * parsed out of that peer's packet; its fields are named after the callback
 * fields they feed. The views are in private/arm9_wbt.h. */
void func_02064554(WBTWork *work, int sel, int idx, s16 a3) {
    WBTPeerRecord *c = &work->peer[idx];
    WBTCommand *o = &work->system_cmd;

    if (sel != WBT_CMD_PREPARE_SEND_DATA && c->cmd_counter == c->last_cmd_counter)
        return;

    o->command = WBT_CMD_SYSTEM_CALLBACK;
    o->peer_bmp = (1 << idx);
    c->last_cmd_counter = c->cmd_counter;
    o->peer_cmd_counter = c->cmd_counter;
    o->result = a3;

    switch (sel) {
    case WBT_CMD_REQ_GET_BLOCK_DONE:
        o->u.blockdone.block_id = c->block_id;
        break;
    case WBT_CMD_REQ_SYNC:
        o->u.sync.num_of_list = c->num_of_list;
        o->u.sync.peer_packet_size = c->peer_packet_size;
        o->u.sync.my_packet_size = c->my_packet_size;
        break;
    case WBT_CMD_REQ_USER_DATA: {
        u8 v = c->user_data_size;
        if (v > WBT_SIZE_USER_DATA) {
            o->u.user_data.size = 0;
            break;
        }
        o->u.user_data.size = v;
        MI_CpuCopy8((const void *)c->user_data, o->u.user_data.data, o->u.user_data.size);
    } break;
    case WBT_CMD_SYSTEM_CALLBACK:
    case WBT_CMD_REQ_ERROR:
        break;
    }

    o->event = sel;
    {
        WBTCallback fn = o->callback;
        if (fn)
            fn(o);
    }
}
