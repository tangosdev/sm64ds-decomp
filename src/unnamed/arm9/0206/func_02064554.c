#include "types.h"
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
 * parsed out of that peer's packet. Its fields are named after the callback
 * fields they feed. cmd_counter is the peer's command counter;
 * last_cmd_counter is the one already reported, so a repeat is dropped. */
typedef struct WBTCommand {
    s32 command;              /* 0x00 */
    s32 event;                /* 0x04 */
    u16 target_bmp;           /* 0x08 */
    u16 peer_bmp;             /* 0x0a */
    u8 my_cmd_counter;        /* 0x0c */
    u8 peer_cmd_counter;      /* 0x0d */
    s16 result;               /* 0x0e */
    void (*callback)(char *); /* 0x10 */
    union {
        struct {
            s16 num_of_list;      /* 0x14 */
            s16 peer_packet_size; /* 0x16 */
            s16 my_packet_size;   /* 0x18 */
        } sync;
        struct {
            s32 block_id; /* 0x14 */
        } blockdone;
        struct {
            u8 data[9]; /* 0x14 */
            u8 size;    /* 0x1d */
        } user_data;
    } u;
} WBTCommand;

typedef struct WBTPeerRecord {
    u8 pad_00[2];
    u8 cmd_counter;       /* 0x02 */
    u8 last_cmd_counter;  /* 0x03 */
    s16 my_packet_size;   /* 0x04 */
    s16 peer_packet_size; /* 0x06 */
    u8 pad_08[2];
    s16 num_of_list; /* 0x0a */
    u8 pad_0c;
    u8 user_data_size; /* 0x0d */
    u8 pad_0e[2];
    s32 block_id; /* 0x10 */
    u8 pad_14[0xc];
    s32 user_data; /* 0x20 -- the payload's address in the packet */
    u8 pad_24[0x44];
} WBTPeerRecord;

typedef struct WBTWork {
    u8 pad_000[0x138];
    WBTCommand system_cmd; /* 0x138 */
    u8 pad_158[0x7c];
    WBTPeerRecord peer[1]; /* 0x1d4 -- indexed by the peer's aid */
} WBTWork;

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
