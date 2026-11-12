#include "types.h"
#include "private/arm9_wbt.h"
#include "nitro/mi/memory.h"
extern void func_02064470(void *self, int code, int idx);
extern void func_02064554(void *self, int sel, int idx, s16 a3);
extern void func_02065ba0(int size);
extern void func_02065bb0(int size);
extern int func_020647d8(void *p, int i);
extern int func_02064674(void *p, int idx, int dst);
extern void func_020647a4(void *p, int value);

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
    int off;
    const u8 *cur;
    char *recv;

    off = aid * sizeof(WBTPeerRecord);
    peer = &work->peer[aid];
    field = 1;
    /* Not &peer->recv: the cartridge forms the address from the
       record offset a second time. */
    recv = (char *)&work->peer[0].recv + off;
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
            case 3:
            {
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
            case 5:
            {
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
            case 7:
            {
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
                case 9:
                {
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
            case 8:
            {
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
                case 7:
                {
                    int cur_cmd;
                    peer->user_data = (int)cur;
                    cur_cmd = work->cmd.command;
                    if ((cur_cmd == 4 || cur_cmd == 6) &&
                        peer->block_id == work->cmd.u.get.block_id &&
                        (work->cmd.target_bmp & (u16)(1 << aid)) &&
                        func_020647d8(recv, peer->seq_no) == 0) {
                        if (func_02064674(recv, peer->seq_no, peer->user_data) == 1) {
                            func_020647a4(recv, peer->seq_no);
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
