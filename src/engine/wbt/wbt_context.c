/* NitroSDK wbt_context.c: the wireless-block-transfer protocol state machine,
 * packet bitmaps and packet writers. The library has two translation units;
 * this is the internal context half, with the public API in wbt_api.c. */
#include "types.h"
#include "nitro/mi/memory.h"
#include "private/arm9_wbt.h"
#include "nitro/wbt.h"

int func_02063718(WBTWork *work, void *buf, int size);
int func_02063980(WBTWork *work, void *buf, int size);
int func_02063ae4(WBTWork *work, int idx, void *buf, int size);
int func_02063bc4(WBTWork *work, int idx, void *buf, int size);
int func_02063d3c(WBTWork *work, int aid, void *buf, int size);
int func_02063da0(WBTWork *work, int idx, void *buf, int size);
int func_02063e08(WBTWork *work, int aid, void *buf, int size);
int func_02063ea8(WBTWork *work, int aid, int buf, int size);
void func_02064470(WBTWork *self, int r1, int r2);
void func_02064554(WBTWork *work, int sel, int idx, s16 a3);
int func_02064674(int *p, int idx, int dst);
int func_0206470c(WBTPacketBitmap *pkt_bmp);
void func_020647a4(char *self, int value);
int func_020647d8(WBTPacketBitmap *p, int i);
void func_020647fc(WBTPacketBitmap *pkt, int length, unsigned int *bitmap, unsigned char *buffer);
char *func_02064888(unsigned int id, int idx);
int func_0206491c(unsigned int key, int val);
int func_02064a28(unsigned char *dst, unsigned short v, unsigned char b);
int func_02064a94(unsigned char *dst, unsigned short a1, unsigned char a2, const void *a3,
                  unsigned char a4);
int func_02064b2c(unsigned char *dst, unsigned short b, int c, int d);
int func_02064bac(unsigned char *dst, unsigned short a1, unsigned char a2, unsigned int a3);
int func_02064c2c(u8 *dst, u16 a, u8 b, int c, int d, const void *p, int n);
int func_02064cd8(unsigned char *dst, unsigned short a1, unsigned char a2, unsigned int a3,
                  unsigned int a4);
int func_02064d6c(u8 *dst, u16 a, u8 b, int c, int d, const void *p, int n);
int func_02064e18(unsigned char *dst, unsigned short a1, unsigned char a2, unsigned int a3,
                  unsigned int a4);
int func_02064eac(unsigned char *dst, unsigned short a1, unsigned char a2, short a3, int a4,
                  int a5);
int func_02064f54(unsigned char *dst, unsigned short b, unsigned char c, int d, int e);
int func_02064fe8(void *dst, unsigned short b, char c);

int func_02064fe8(void *dst, unsigned short b, char c) {
    char *p;
    char *q;
    unsigned char one = 1;
    unsigned short s = b;
    MI_CpuCopy8(&one, dst, 1);
    p = dst + 1;
    MI_CpuCopy8(&s, p, 2);
    q = p + 2;
    MI_CpuCopy8(&c, q, 1);
    return q + 1 - dst;
}

int func_02064f54(unsigned char *dst, unsigned short b, unsigned char c, int d, int e) {
    unsigned char tag = 2;
    short val = b;
    unsigned char *p1, *p2, *p3, *p4;
    MI_CpuCopy8(&tag, dst, 1);
    p1 = dst + 1;
    MI_CpuCopy8(&val, p1, 2);
    p2 = p1 + 2;
    MI_CpuCopy8(&c, p2, 1);
    p3 = p2 + 1;
    MI_CpuCopy8(&d, p3, 2);
    p4 = p3 + 2;
    MI_CpuCopy8(&e, p4, 2);
    return (p4 + 2) - dst;
}

int func_02064eac(unsigned char *dst, unsigned short a1, unsigned char a2, short a3, int a4,
                  int a5) {
    unsigned char hdr = 3;
    unsigned short s = a1;
    unsigned char *p1, *p2, *p3, *p4, *p5;
    MI_CpuCopy8(&hdr, dst, 1);
    p1 = dst + 1;
    MI_CpuCopy8(&s, p1, 2);
    p2 = p1 + 2;
    MI_CpuCopy8(&a2, p2, 1);
    p3 = p2 + 1;
    MI_CpuCopy8(&a3, p3, 2);
    p4 = p3 + 2;
    MI_CpuCopy8(&a4, p4, 2);
    p5 = p4 + 2;
    MI_CpuCopy8(&a5, p5, 2);
    return (p5 + 2) - dst;
}

int func_02064e18(unsigned char *dst, unsigned short a1, unsigned char a2, unsigned int a3,
                  unsigned int a4) {
    unsigned char hdr = 4;
    unsigned short s = a1;
    unsigned char *p1, *p2, *p3, *p4;
    MI_CpuCopy8(&hdr, dst, 1);
    p1 = dst + 1;
    MI_CpuCopy8(&s, p1, 2);
    p2 = p1 + 2;
    MI_CpuCopy8(&a2, p2, 1);
    p3 = p2 + 1;
    MI_CpuCopy8(&a3, p3, 4);
    p4 = p3 + 4;
    MI_CpuCopy8(&a4, p4, 4);
    return (p4 + 4) - dst;
}

int func_02064d6c(u8 *dst, u16 a, u8 b, int c, int d, const void *p, int n) {
    u32 len;
    u8 kind = 5;
    u16 a16 = a;
    u8 *p1, *p2, *p3, *p4, *p5;
    MI_CpuCopy8(&kind, dst, 1);
    p1 = dst + 1;
    MI_CpuCopy8(&a16, p1, 2);
    p2 = p1 + 2;
    MI_CpuCopy8(&b, p2, 1);
    p3 = p2 + 1;
    MI_CpuCopy8(&c, p3, 4);
    p4 = p3 + 4;
    MI_CpuCopy8(&d, p4, 4);
    p5 = p4 + 4;
    len = n;
    MI_CpuCopy8(p, p5, len);
    return (p5 + len) - dst;
}

int func_02064cd8(unsigned char *dst, unsigned short a1, unsigned char a2, unsigned int a3,
                  unsigned int a4) {
    unsigned char hdr = 6;
    unsigned short s = a1;
    unsigned char *p1, *p2, *p3, *p4;
    MI_CpuCopy8(&hdr, dst, 1);
    p1 = dst + 1;
    MI_CpuCopy8(&s, p1, 2);
    p2 = p1 + 2;
    MI_CpuCopy8(&a2, p2, 1);
    p3 = p2 + 1;
    MI_CpuCopy8(&a3, p3, 4);
    p4 = p3 + 4;
    MI_CpuCopy8(&a4, p4, 4);
    return (p4 + 4) - dst;
}

int func_02064c2c(u8 *dst, u16 a, u8 b, int c, int d, const void *p, int n) {
    u32 len;
    u8 kind = 7;
    u16 a16 = a;
    u8 *p1, *p2, *p3, *p4, *p5;
    MI_CpuCopy8(&kind, dst, 1);
    p1 = dst + 1;
    MI_CpuCopy8(&a16, p1, 2);
    p2 = p1 + 2;
    MI_CpuCopy8(&b, p2, 1);
    p3 = p2 + 1;
    MI_CpuCopy8(&c, p3, 4);
    p4 = p3 + 4;
    MI_CpuCopy8(&d, p4, 4);
    p5 = p4 + 4;
    len = n;
    MI_CpuCopy8(p, p5, len);
    return (p5 + len) - dst;
}

int func_02064bac(unsigned char *dst, unsigned short a1, unsigned char a2, unsigned int a3) {
    unsigned char hdr = 8;
    unsigned short s = a1;
    unsigned char *p1, *p2, *p3;
    MI_CpuCopy8(&hdr, dst, 1);
    p1 = dst + 1;
    MI_CpuCopy8(&s, p1, 2);
    p2 = p1 + 2;
    MI_CpuCopy8(&a2, p2, 1);
    p3 = p2 + 1;
    MI_CpuCopy8(&a3, p3, 4);
    return (p3 + 4) - dst;
}

int func_02064b2c(unsigned char *dst, unsigned short b, int c, int d) {
    unsigned char tag = 9;
    short val = b;
    unsigned char *c1, *c2, *c3;
    MI_CpuCopy8(&tag, dst, 1);
    c1 = dst + 1;
    MI_CpuCopy8(&val, c1, 2);
    c2 = c1 + 2;
    MI_CpuCopy8(&c, c2, 1);
    c3 = c2 + 1;
    MI_CpuCopy8(&d, c3, 4);
    return c3 + 4 - dst;
}

int func_02064a94(unsigned char *dst, unsigned short a1, unsigned char a2, const void *a3,
                  unsigned char a4) {
    unsigned char hdr = 10;
    unsigned short s = a1;
    unsigned char *p1, *p2, *p3, *p4;
    MI_CpuCopy8(&hdr, dst, 1);
    p1 = dst + 1;
    MI_CpuCopy8(&s, p1, 2);
    p2 = p1 + 2;
    MI_CpuCopy8(&a2, p2, 1);
    p3 = p2 + 1;
    MI_CpuCopy8(&a4, p3, 1);
    p4 = p3 + 1;
    MI_CpuCopy8(a3, p4, 9);
    return (p4 + 9) - dst;
}

int func_02064a28(unsigned char *dst, unsigned short v, unsigned char b) {
    unsigned char tag = 0xb;
    short val = v;
    unsigned char *c1, *c2;
    MI_CpuCopy8(&tag, dst, 1);
    c1 = dst + 1;
    MI_CpuCopy8(&val, c1, 2);
    c2 = c1 + 2;
    MI_CpuCopy8(&b, c2, 1);
    return c2 + 1 - dst;
}

extern WBTBlockInfoList *func_02065ae0(void);
extern int func_02065bc0(void);
int data_0209a064 = -1;
int data_0209a068 = -1;
int data_0209a06c = -1;

int func_0206491c(unsigned int key, int val) {
    int n;
    WBTBlockInfoList *node;
    int q;

    node = func_02065ae0();
    if (key < 0x3e8)
        return val;

    while (node != 0) {
        if (node->data_info.id == key) {
            if (key == data_0209a068) {
                n = 0;
                while (val == data_0209a064 || val == data_0209a06c) {
                    val += 1;
                    q = func_02065bc0();
                    q = (node->data_info.block_size + q - 1) / func_02065bc0();
                    q -= 1;
                    if (val > q)
                        val = 0;
                    n += 1;
                    if (n > 2)
                        break;
                }
            }

            data_0209a068 = key;
            data_0209a06c = data_0209a064;
            data_0209a064 = val;
            return val;
        }
        node = node->next;
    }

    return -1;
}

extern WBTBlockInfoList *func_02065ae0(void);
extern int func_02065bc0(void);

char *func_02064888(unsigned int id, int idx) {
    WBTBlockInfoList *node = func_02065ae0();

    if (id < 0x3e8) {
        unsigned int i = 0;
        for (;;) {
            if (i == id)
                return (char *)node + idx * func_02065bc0();
            i++;
            if (node != 0)
                node = node->next;
            else
                return 0;
        }
    }

    while (node != 0) {
        if (node->data_info.id == id) {
            char *base = node->data_ptr;
            return base + idx * func_02065bc0();
        }
        node = node->next;
    }

    return 0;
}

extern int func_02065bd0(void);

/* NitroSDK WBT: reset one peer's packet bitmap for a transfer of length
 * bytes into buffer: the packet count (length over the packet size,
 * rounded up), zeroed cursors, then the bitmap words cleared. */
void func_020647fc(WBTPacketBitmap *pkt, int length, unsigned int *bitmap, unsigned char *buffer) {
    int d = func_02065bd0();
    int r;
    pkt->length = length;
    pkt->next = 0;
    pkt->current = 0;
    pkt->recv_count = 0;
    pkt->count = length / d + (length % d != 0);
    pkt->bitmap = bitmap;
    pkt->buffer = buffer;
    r = WBT_CalcPacketbitmapSize(length);
    MI_CpuClear8(bitmap, r);
}

/* Whether packet i is marked received in the bitmap (func_020647a4 sets the
 * bit). */
int func_020647d8(WBTPacketBitmap *p, int i) {
    int w = p->bitmap[i >> 5];
    i = 1 << (i & 0x1f);
    return (w & i) != 0;
}

/* Mark packet value received: count it, remember it as the latest, and set
 * its bit. func_02063ea8 calls it once func_02064674 has copied the packet
 * into the buffer. */
void func_020647a4(char *self, int value) {
    WBTPacketBitmap *pkt_bmp = (WBTPacketBitmap *)self;
    pkt_bmp->recv_count += 1;
    pkt_bmp->current = value;

    pkt_bmp->bitmap[value >> 5] |= 1 << (value & 0x1f);
}

/* The next packet pkt_bmp still lacks, searching on from the one received
 * last (current) and wrapping at count; it is kept in next and returned, or
 * -1 once every bit is set. */
int func_0206470c(WBTPacketBitmap *pkt_bmp) {
    int idx = pkt_bmp->current + 1;
    int start;
    int bit;
    unsigned int *word;
    if (idx >= pkt_bmp->count)
        idx = 0;
    word = pkt_bmp->bitmap + (idx >> 5);
    start = idx;
    bit = idx & 0x1f;
    /* found/fail gotos kept. The ROM leaves the loop with a forward branch over the found
       tail to the fail tail, laid out in label order (0x98). Measured: returning from
       inside the loop, with a for, a break or a do-while, 0x90; breaking to the found
       tail with `return -1` in the loop, 0x94 (the fail return lands straight after
       the loop); while (*word & (1 << bit)) with the fail return inside, 0xa0. */
    for (;;) {
        if (!(*word & (1 << bit)))
            goto found;
        idx++;
        if (idx < pkt_bmp->count) {
            bit++;
            if (bit > 0x1f) {
                bit = 0;
                word++;
            }
        } else {
            idx = 0;
            bit = 0;
            word = pkt_bmp->bitmap;
        }
        if (idx == start)
            goto fail;
    }
found:
    pkt_bmp->next = idx;
    return idx;
fail:
    return -1;
}

extern int func_02065bd0(void);

int func_02064674(int *p, int idx, int dst) {
    int n = func_02065bd0();
    int cnt = p[1];
    int base = p[6];
    if (idx < cnt) {
        if (idx == cnt - 1) {
            int rem = p[0] % n;
            if (rem != 0)
                MI_CpuCopy8((const void *)dst, (void *)(idx * n + base), rem);
            else
                MI_CpuCopy8((const void *)dst, (void *)(idx * n + base), n);
        } else {
            MI_CpuCopy8((const void *)dst, (void *)(idx * n + base), n);
        }
    } else {
        return 0;
    }
    return 1;
}

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

/* NitroSDK WBT: peer r2 replied to the running command (self->cmd) with
 * event r1. When the reply carries the command's counter, drop the peer from
 * target_bmp, copy RES_SYNC's sync sizes out of the peer's record and raise the
 * command's callback; once no target is left the command slot is cleared. */
void func_02064470(WBTWork *self, int r1, int r2) {
    WBTPeerRecord *e = &self->peer[r2];
    unsigned short mask = 1 << r2;

    self->cmd.peer_bmp = mask;
    if (self->cmd.my_cmd_counter != e->cmd_counter)
        return;
    self->cmd.peer_cmd_counter = e->cmd_counter;
    if ((self->cmd.target_bmp & mask) != 0) {
        self->cmd.target_bmp &= ~mask;

        switch (r1) {
        case WBT_CMD_REQ_NONE:
        case WBT_CMD_REQ_WAIT:
        case WBT_CMD_REQ_SYNC:
        case WBT_CMD_REQ_GET_BLOCK:
        case WBT_CMD_RES_GET_BLOCK:
        case WBT_CMD_REQ_GET_BLOCKINFO:
        case WBT_CMD_RES_GET_BLOCKINFO:
        case WBT_CMD_REQ_GET_BLOCK_DONE:
        case WBT_CMD_RES_GET_BLOCK_DONE:
        case WBT_CMD_REQ_USER_DATA:
        case WBT_CMD_RES_USER_DATA:
            break;
        case WBT_CMD_RES_SYNC:
            self->cmd.u.sync.num_of_list = e->num_of_list;
            self->cmd.u.sync.peer_packet_size = e->peer_packet_size;
            self->cmd.u.sync.my_packet_size = e->my_packet_size;
            break;
        }

        self->cmd.event = r1;
        if (self->cmd.callback != 0) {
            self->cmd.callback(&self->cmd);
        }
    }
    if (self->cmd.target_bmp == 0)
        self->cmd.command = WBT_CMD_REQ_NONE;
}

extern void func_02065ba0(int size);
extern void func_02065bb0(int size);

/* Whether this console's aid is set in a packet's target bitmap. */
static inline int IsTargeted(u16 bmp) {
    if (WBT_GetOwnAid() == -1) {
        return 0;
    } else if (bmp & (1 << WBT_GetOwnAid())) {
        return 1;
    } else {
        return 0;
    }
}

/* NitroSDK WBT (wireless block transfer): parse one peer's packet into its
 * 0x68-byte record (peer[aid], +0x1d4) and act on it. buf is the packet,
 * size its length; returns the bytes consumed. The parse runs as a field
 * state machine: command (1), target bitmap (2), command counter (3), then by
 * command the list count (4), packet sizes (5), block id (7), sequence number
 * (8), user-data size (10) and the payload address (9). state goes 1 while
 * parsing and 2 once the record is complete. The record's recv is the
 * per-peer packet bitmap that func_020647d8 / func_02064674 / func_020647a4
 * test, store and mark. The views are in private/arm9_wbt.h. */
int func_02063ea8(WBTWork *work, int aid, int buf, int size) {
    int consumed;
    int field;
    WBTPeerRecord *peer;
    const u8 *cur;
    WBTPeerRecv *recv;

    peer = &work->peer[aid];
    field = 1;
    recv = &work->peer[aid].recv;
    consumed = 0;
    cur = (const u8 *)buf;
    peer->state = 1;
    if (size == 0) {
        return 0;
    }
    if (size > 0) {
        do {
            switch (field) {
            case 1:
                MI_CpuCopy8(cur, &peer->command, 1);
                cur += 1;
                consumed += 1;
                field = 2;
                break;
            case 2:
                MI_CpuCopy8(cur, &peer->target_bmp, 2);
                cur += 2;
                consumed += 2;
                if (!IsTargeted(peer->target_bmp)) {
                    return consumed;
                }
                field = 3;
                break;
            case 3: {
                u8 cmd;
                MI_CpuCopy8(cur, &peer->cmd_counter, 1);
                cmd = peer->command;
                cur += 1;
                consumed += 1;
                switch (cmd) {
                case 1:
                    return consumed;
                case 2:
                    field = 5;
                    break;
                case 3:
                    field = 4;
                    break;
                case 11:
                    if (work->cmd.command == 0xA) {
                        func_02064470(work, cmd, aid);
                    }
                    return consumed;
                case 10:
                    field = 0xA;
                    break;
                default:
                case 0:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    field = 7;
                    break;
                }
                break;
            }
            case 10:
                MI_CpuCopy8(cur, &peer->user_data_size, 1);
                cur += 1;
                consumed += 1;
                field = 9;
                break;
            case 9:
                peer->user_data = (int)cur;
                func_02064554(work, peer->command, aid, 0);
                peer->state = 2;
                return consumed + 9;
            case 4:
                MI_CpuCopy8(cur, &peer->num_of_list, 2);
                cur += 2;
                consumed += 2;
                if (peer->command == 3) {
                    field = 5;
                }
                break;
            case 5: {
                u8 cmd;
                MI_CpuCopy8(cur, &peer->peer_packet_size, 2);
                MI_CpuCopy8(cur + 2, &peer->my_packet_size, 2);
                if (WBT_GetOwnAid() != 0) {
                    func_02065ba0(peer->my_packet_size);
                    func_02065bb0(peer->peer_packet_size);
                }
                cmd = peer->command;
                switch (cmd) {
                case 2:
                    func_02064554(work, cmd, aid, 0);
                    peer->state = 2;
                    return consumed + 4;
                case 3:
                    if (work->cmd.command == 2) {
                        func_02064470(work, cmd, aid);
                    }
                    return consumed + 4;
                default:
                    func_02064554(work, 0xF, aid, 3);
                    return consumed + 4;
                }
            }
            case 7: {
                u8 cmd;
                MI_CpuCopy8(cur, &peer->block_id, 4);
                cmd = peer->command;
                cur += 4;
                consumed += 4;
                switch (cmd) {
                case 8:
                    func_02064554(work, cmd, aid, 0);
                    peer->state = 2;
                    return consumed;
                case 9: {
                    int cur_cmd = work->cmd.command;
                    if (cur_cmd == 4) {
                        if (work->cmd.u.get.block_id == peer->block_id) {
                            func_02064470(work, 5, aid);
                        }
                    } else if (cur_cmd == 6 && work->cmd.u.get.block_id == peer->block_id) {
                        func_02064470(work, 7, aid);
                    }
                    return consumed;
                }
                case 4:
                case 5:
                case 6:
                case 7:
                    field = 8;
                    break;
                default:
                case 0:
                case 1:
                case 2:
                case 3:
                    func_02064554(work, 0xF, aid, 3);
                    break;
                }
                break;
            }
            case 8: {
                u8 cmd;
                MI_CpuCopy8(cur, &peer->seq_no, 4);
                cmd = peer->command;
                cur += 4;
                consumed += 4;
                switch (cmd) {
                case 4:
                case 6:
                    peer->state = 2;
                    return consumed;
                case 5:
                case 7: {
                    int cur_cmd;
                    peer->user_data = (int)cur;
                    cur_cmd = work->cmd.command;
                    if ((cur_cmd == 4 || cur_cmd == 6) &&
                        peer->block_id == work->cmd.u.get.block_id &&
                        (work->cmd.target_bmp & (u16)(1 << aid)) &&
                        func_020647d8((WBTPacketBitmap *)recv, peer->seq_no) == 0) {
                        if (func_02064674((int *)recv, peer->seq_no, peer->user_data) == 1) {
                            func_020647a4((char *)recv, peer->seq_no);
                        } else {
                            func_02064554(work, 0xF, aid, 6);
                        }
                    }
                    return consumed;
                }
                default:
                case 0:
                case 1:
                case 2:
                case 3:
                    func_02064554(work, 0xF, aid, 3);
                    break;
                }
                break;
            }
            default:
            case 0:
            case 6:
                func_02064554(work, 0xF, aid, 4);
                break;
            }
        } while (consumed < size);
    }
    func_02064554(work, 0xF, aid, 5);
    return consumed;
}

int func_02065bc0(void);
int func_02065bd0(void);

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

int func_02063d3c(WBTWork *work, int aid, void *buf, int size) {
    WBTPeerRecord *peer = &work->peer[aid];
    unsigned short mask = (1 << aid);
    int ret = 0;
    if (peer->state == 2 && peer->command == WBT_CMD_REQ_USER_DATA) {
        ret = func_02064a28(buf, mask, peer->cmd_counter);
        peer->state = 1;
    }
    return ret;
}

extern WBTBlockInfoList *func_02065a94(int key);
extern int func_02065bc0(void);

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
        u16 r7;
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

extern int func_02065bc0(void);

int func_02063ae4(WBTWork *work, int idx, void *buf, int size) {
    WBTPeerRecord *e = &work->peer[idx];
    int r7 = 0;
    if (e->state == 2) {
        u16 r5;
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

extern int func_02065bc0(void);
extern int func_02065bd0(void);

/* NitroSDK WBT: write the running command's next request packet into buf.
 * SYNC and USER_DATA resend the request. GET_BLOCK and GET_BLOCKINFO ask each
 * target peer for the next packet its receive bitmap still lacks
 * (func_0206470c), or send it REQ_GET_BLOCK_DONE (func_02064bac) once none
 * is missing. The receive state steps a whole peer record at a time, as in
 * func_02063718: indexing work->peer[i] swaps two loop registers, and
 * stepping a WBTPeerRecord pointer adds 0x24 before each call. */
int func_02063980(WBTWork *work, void *buf, int size) {
    int ret = 0;
    int i;

    switch (work->cmd.command) {
    case WBT_CMD_REQ_SYNC:
        ret = func_02064f54(buf, work->cmd.target_bmp, work->cmd.my_cmd_counter, func_02065bc0(),
                            func_02065bd0());
        break;
    case WBT_CMD_REQ_GET_BLOCK:
    case WBT_CMD_REQ_GET_BLOCKINFO: {
        WBTPeerRecv *recv = &work->peer[0].recv;
        for (i = 0; i < 16; i++, recv = (WBTPeerRecv *)((char *)recv + sizeof(WBTPeerRecord))) {
            int bit = 1 << i;
            if (work->cmd.target_bmp & (u16)bit) {
                int v = func_0206470c(&recv->pkt_bmp);
                if (v == -1) {
                    ret =
                        func_02064bac(buf, bit, work->cmd.my_cmd_counter, work->cmd.u.get.block_id);
                } else {
                    u16 mask = work->cmd.target_bmp;
                    if (work->cmd.command == WBT_CMD_REQ_GET_BLOCK)
                        ret = func_02064e18(buf, mask, work->cmd.my_cmd_counter,
                                            work->cmd.u.get.block_id, v);
                    else
                        ret = func_02064cd8(buf, mask, work->cmd.my_cmd_counter,
                                            work->cmd.u.get.block_id, v);
                }
                if (ret != 0)
                    return ret;
            }
        }
        break;
    }
    case WBT_CMD_REQ_USER_DATA:
        ret = func_02064a94(buf, work->cmd.target_bmp, work->cmd.my_cmd_counter,
                            work->cmd.u.user_data.data, work->cmd.u.user_data.size);
        break;
    }
    return ret;
}

/* NitroSDK WBT (wireless block transfer): start the command the application
 * queued. When no command is running (cmd.command == 0) and one is queued,
 * the queued 0x9c-byte WBTCommand is copied over the running one, given the
 * next command counter, and its first request is sent; the queue slot is then
 * cleared. Commands: 2 REQ_SYNC, 4 REQ_GET_BLOCK, 6 REQ_GET_BLOCKINFO,
 * 10 REQ_USER_DATA. A block (or block-info) request first resets each target
 * peer's receive bitmap: GET_BLOCK into the caller's buffer and bitmap
 * tables, GET_BLOCKINFO into the peer record's own bitmap words.
 * The WBTCommand / WBTWork views are in private/arm9_wbt.h. */

extern void func_02065bf4(void);
extern int func_02065c1c(void);
extern int func_02065bc0(void);
extern int func_02065bd0(void);
extern int func_02064f54(unsigned char *dst, unsigned short target_bmp, unsigned char cmd_counter,
                         int b, int c);
extern void func_020647fc(WBTPacketBitmap *pkt, int length, unsigned int *bitmap,
                          unsigned char *buffer);
extern int func_02064e18(unsigned char *dst, unsigned short target_bmp, unsigned char cmd_counter,
                         unsigned int block_id, unsigned int seq);
extern int func_02064cd8(unsigned char *dst, unsigned short target_bmp, unsigned char cmd_counter,
                         unsigned int block_id, unsigned int seq);
extern int func_02064a94(unsigned char *dst, unsigned short target_bmp, unsigned char cmd_counter,
                         const void *data, unsigned char size);

int func_02063718(WBTWork *work, void *buf, int size) {
    int t;
    int ret = 0;
    WBTCommand *st = (WBTCommand *)((long long)(unsigned int)&work->queued_cmd);
    int b;
    int i;
    WBTPeerRecv *recv;
    if (work->cmd.command != WBT_CMD_REQ_NONE)
        return ret;
    if (st->command == WBT_CMD_REQ_NONE)
        return ret;
    func_02065bf4();
    t = func_02065c1c();
    switch (st->command) {
    case WBT_CMD_REQ_SYNC:
        MI_CpuCopy32(st, &work->cmd, sizeof(WBTCommand));
        work->cmd.my_cmd_counter = t;
        ret = func_02065bc0();
        b = func_02065bd0();
        ret = func_02064f54(buf, work->cmd.target_bmp, work->cmd.my_cmd_counter, ret, b);
        break;
    case WBT_CMD_REQ_GET_BLOCKINFO:
        MI_CpuCopy32(st, &work->cmd, sizeof(WBTCommand));
        work->cmd.my_cmd_counter = t;
        recv = &work->peer[0].recv;
        for (i = 0; i < 16; i++, recv = (WBTPeerRecv *)((char *)recv + sizeof(WBTPeerRecord))) {
            if ((work->cmd.target_bmp >> i) & 1)
                func_020647fc(&recv->pkt_bmp, work->cmd.u.get.recv_data_size, recv->blockinfo_bits,
                              work->cmd.u.get.recv_buf_table[i]);
        }
        if (((WBTCommand *)((unsigned int)&work->queued_cmd))->command == WBT_CMD_REQ_GET_BLOCK)
            ret = func_02064e18(buf, work->cmd.target_bmp, work->cmd.my_cmd_counter,
                                work->cmd.u.get.block_id, 0);
        else
            ret = func_02064cd8(buf, work->cmd.target_bmp, work->cmd.my_cmd_counter,
                                work->cmd.u.get.block_id, 0);
        break;
    case WBT_CMD_REQ_GET_BLOCK:
        MI_CpuCopy32(st, &work->cmd, sizeof(WBTCommand));
        work->cmd.my_cmd_counter = t;
        recv = &work->peer[0].recv;
        for (i = 0; i < 16; i++, recv = (WBTPeerRecv *)((char *)recv + sizeof(WBTPeerRecord))) {
            if ((work->cmd.target_bmp >> i) & 1)
                func_020647fc(&recv->pkt_bmp, work->cmd.u.get.recv_data_size,
                              work->cmd.u.get.pkt_bmp_table[i], work->cmd.u.get.recv_buf_table[i]);
        }
        if (((WBTCommand *)((unsigned int)&work->queued_cmd))->command == WBT_CMD_REQ_GET_BLOCK)
            ret = func_02064e18(buf, work->cmd.target_bmp, work->cmd.my_cmd_counter,
                                work->cmd.u.get.block_id, 0);
        else
            ret = func_02064cd8(buf, work->cmd.target_bmp, work->cmd.my_cmd_counter,
                                work->cmd.u.get.block_id, 0);
        break;
    case WBT_CMD_REQ_USER_DATA:
        MI_CpuCopy32(st, &work->cmd, sizeof(WBTCommand));
        work->cmd.my_cmd_counter = t;
        ret = func_02064a94(buf, work->cmd.target_bmp, work->cmd.my_cmd_counter,
                            work->cmd.u.user_data.data, work->cmd.u.user_data.size);
        break;
    }
    ((WBTCommand *)((unsigned int)&work->queued_cmd))->command = WBT_CMD_REQ_NONE;
    return ret;
}