#include "types.h"
#include "private/arm9_wbt.h"
extern void CpuCopy8(int a, void *b, int c);
extern int func_02065a74(void);
extern void func_02064470(void *self, int code, int idx);
extern void func_02064554(void *self, int sel, int idx, int a3);
extern void func_02065ba0(int v);
extern void func_02065bb0(int v);
extern int func_020647d8(void *p, int i);
extern int func_02064674(void *p, int idx, int dst);
extern void func_020647a4(void *p, int value);

/* NitroSDK WBT (wireless block transfer): parse one peer's packet into its
 * 0x68-byte record (peer[arg1], +0x1d4) and act on it. arg2 is the packet,
 * arg3 its length; returns the bytes consumed. The parse runs as a field
 * state machine: command (1), target bitmap (2), command counter (3), then by
 * command the list count (4), packet sizes (5), block id (7), sequence number
 * (8), user-data size (10) and the payload address (9). state goes 1 while
 * parsing and 2 once the record is complete. The record's recv is the
 * per-peer packet bitmap that func_020647d8 / func_02064674 / func_020647a4
 * test, store and mark. The views are in private/arm9_wbt.h. */
int func_02063ea8(char *arg0, int arg1, int arg2, int arg3)
{
    WBTWork *work = (WBTWork *)arg0;
    int r7;
    int r6;
    WBTPeerRecord *peer;
    int off;
    int r8;
    int sp0;
    char *sp4;

    off = arg1 * sizeof(WBTPeerRecord);
    sp0 = arg3;
    peer = &work->peer[arg1];
    r6 = 1;
    sp4 = (char *)&work->peer[0].recv + off;
    r7 = 0;
    r8 = arg2;
    peer->state = 1;
    if (sp0 == 0) {
        return r7;
    }
    if (sp0 > 0) {
        int sp8 = 1;
        int spC = 0;
        int sp10 = 1;
        int sp14 = 0;
        int sp18 = 3;
        int sp1C = 1;
        int sp20 = 5;
        int sp24 = 0xA;
        int sp28 = 7;
        int sp2C = 1;
        int sp30 = 9;
        int sp34 = 8;
        int sp38 = 0xF;
        int r4 = 4;
        int fp = 2;

        for (;;) {
            switch (r6) {
            case 1:
                CpuCopy8(r8, &peer->command, sp8);
                r8 += 1;
                r7 += 1;
                r6 = fp;
                break;
            case 2:
            {
                u16 t;
                int v;
                CpuCopy8(r8, &peer->target_bmp, fp);
                r8 += 2;
                r7 += 2;
                t = peer->target_bmp;
                if (func_02065a74() == -1) {
                    v = spC;
                } else if (t & (sp10 << func_02065a74())) {
                    v = sp10;
                } else {
                    v = sp14;
                }
                if (v == 0) {
                    return r7;
                }
                r6 = sp18;
                break;
            }
            case 3:
            {
                u8 v;
                CpuCopy8(r8, &peer->cmd_counter, sp1C);
                v = peer->command;
                r8 += 1;
                r7 += 1;
                switch (v) {
                case 1:
                    return r7;
                case 2:
                    r6 = sp20;
                    break;
                case 3:
                    r6 = 4;
                    break;
                case 11:
                    if (work->cmd.command == 0xA) {
                        func_02064470(arg0, v, arg1);
                    }
                    return r7;
                case 10:
                    r6 = sp24;
                    break;
                default:
                case 0:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    r6 = sp28;
                    break;
                }
                break;
            }
            case 10:
                CpuCopy8(r8, &peer->user_data_size, sp2C);
                r8 += 1;
                r7 += 1;
                r6 = sp30;
                break;
            case 9:
                peer->user_data = r8;
                func_02064554(arg0, peer->command, arg1, 0);
                peer->state = 2;
                return r7 + 9;
            case 4:
                CpuCopy8(r8, &peer->num_of_list, fp);
                r8 += 2;
                r7 += 2;
                if (peer->command == 3) {
                    r6 = sp20;
                }
                break;
            case 5:
            {
                u8 v;
                CpuCopy8(r8, &peer->peer_packet_size, 2);
                CpuCopy8(r8 + 2, &peer->my_packet_size, 2);
                if (func_02065a74() != 0) {
                    func_02065ba0(peer->my_packet_size);
                    func_02065bb0(peer->peer_packet_size);
                }
                v = peer->command;
                switch (v) {
                case 2:
                    func_02064554(arg0, v, arg1, 0);
                    peer->state = 2;
                    return r7 + 4;
                case 3:
                    if (work->cmd.command == 2) {
                        func_02064470(arg0, v, arg1);
                    }
                    return r7 + 4;
                default:
                    func_02064554(arg0, 0xF, arg1, 3);
                    return r7 + 4;
                }
            }
            case 7:
            {
                u8 v;
                CpuCopy8(r8, &peer->block_id, r4);
                v = peer->command;
                r8 += 4;
                r7 += 4;
                switch (v) {
                case 8:
                    func_02064554(arg0, v, arg1, 0);
                    peer->state = 2;
                    return r7;
                case 9:
                {
                    int st = work->cmd.command;
                    if (st == 4) {
                        if (work->cmd.u.get.block_id == peer->block_id) {
                            func_02064470(arg0, 5, arg1);
                        }
                    } else if (st == 6 && work->cmd.u.get.block_id == peer->block_id) {
                        func_02064470(arg0, 7, arg1);
                    }
                    return r7;
                }
                case 4:
                case 5:
                case 6:
                case 7:
                    r6 = sp34;
                    break;
                default:
                case 0:
                case 1:
                case 2:
                case 3:
                    func_02064554(arg0, sp38, arg1, sp18);
                    break;
                }
                break;
            }
            case 8:
            {
                u8 v;
                CpuCopy8(r8, &peer->seq_no, r4);
                v = peer->command;
                r8 += 4;
                r7 += 4;
                switch (v) {
                case 4:
                case 6:
                    peer->state = 2;
                    return r7;
                case 5:
                case 7:
                {
                    int st;
                    peer->user_data = r8;
                    st = work->cmd.command;
                    if ((st == 4 || st == 6) && peer->block_id == work->cmd.u.get.block_id &&
                        (work->cmd.target_bmp & (u16)(1 << arg1)) &&
                        func_020647d8(sp4, peer->seq_no) == 0) {
                        if (func_02064674(sp4, peer->seq_no, peer->user_data) == 1) {
                            func_020647a4(sp4, peer->seq_no);
                        } else {
                            func_02064554(arg0, 0xF, arg1, 6);
                        }
                    }
                    return r7;
                }
                default:
                case 0:
                case 1:
                case 2:
                case 3:
                    func_02064554(arg0, sp38, arg1, sp18);
                    break;
                }
                break;
            }
            default:
            case 0:
            case 6:
                func_02064554(arg0, sp38, arg1, r4);
                break;
            }
            if (r7 >= sp0) {
                break;
            }
        }
    }
    func_02064554(arg0, 0xF, arg1, 5);
    return r7;
}
