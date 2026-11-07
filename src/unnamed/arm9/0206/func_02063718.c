#include "types.h"
#include "private/arm9_wbt.h"

/* NitroSDK WBT (wireless block transfer): start the command the application
 * queued. When no command is running (cmd.command == 0) and one is queued,
 * the queued 0x9c-byte WBTCommand is copied over the running one, given the
 * next command counter, and its first request is sent; the queue slot is then
 * cleared. Commands: 2 REQ_SYNC, 4 REQ_GET_BLOCK, 6 REQ_GET_BLOCKINFO,
 * 10 REQ_USER_DATA. A block (or block-info) request first resets each target
 * peer's receive bitmap: GET_BLOCK into the caller's buffer and bitmap
 * tables, GET_BLOCKINFO into the peer record's own bitmap words.
 * The WBTCommand / WBTWork views are in private/arm9_wbt.h. */

extern void MultiCopy_Int(void *src, void *dst, int n); /* loads through r0, stores through r1 */
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

int func_02063718(WBTWork *work, unsigned char *q) {
    int t;
    int ret = 0;
    WBTCommand *st = (WBTCommand *)((long long)(unsigned int)&work->queued_cmd);
    int b;
    int i;
    WBTPeerRecv *recv;
    if (work->cmd.command != 0)
        return ret;
    if (st->command == 0)
        return ret;
    func_02065bf4();
    t = func_02065c1c();
    switch (st->command) {
    case 2:
        MultiCopy_Int(st, &work->cmd, sizeof(WBTCommand));
        work->cmd.my_cmd_counter = t;
        ret = func_02065bc0();
        b = func_02065bd0();
        ret = func_02064f54(q, work->cmd.target_bmp, work->cmd.my_cmd_counter, ret, b);
        break;
    case 6:
        MultiCopy_Int(st, &work->cmd, sizeof(WBTCommand));
        work->cmd.my_cmd_counter = t;
        recv = &work->peer[0].recv;
        for (i = 0; i < 0x10; i++, recv = (WBTPeerRecv *)((char *)recv + sizeof(WBTPeerRecord))) {
            if ((work->cmd.target_bmp >> i) & 1)
                func_020647fc(&recv->pkt_bmp, work->cmd.u.get.recv_data_size, recv->blockinfo_bits,
                              work->cmd.u.get.recv_buf_table[i]);
        }
        if (((WBTCommand *)((unsigned int)&work->queued_cmd))->command == 4)
            ret = func_02064e18(q, work->cmd.target_bmp, work->cmd.my_cmd_counter,
                                work->cmd.u.get.block_id, 0);
        else
            ret = func_02064cd8(q, work->cmd.target_bmp, work->cmd.my_cmd_counter,
                                work->cmd.u.get.block_id, 0);
        break;
    case 4:
        MultiCopy_Int(st, &work->cmd, sizeof(WBTCommand));
        work->cmd.my_cmd_counter = t;
        recv = &work->peer[0].recv;
        for (i = 0; i < 0x10; i++, recv = (WBTPeerRecv *)((char *)recv + sizeof(WBTPeerRecord))) {
            if ((work->cmd.target_bmp >> i) & 1)
                func_020647fc(&recv->pkt_bmp, work->cmd.u.get.recv_data_size,
                              work->cmd.u.get.pkt_bmp_table[i], work->cmd.u.get.recv_buf_table[i]);
        }
        if (((WBTCommand *)((unsigned int)&work->queued_cmd))->command == 4)
            ret = func_02064e18(q, work->cmd.target_bmp, work->cmd.my_cmd_counter,
                                work->cmd.u.get.block_id, 0);
        else
            ret = func_02064cd8(q, work->cmd.target_bmp, work->cmd.my_cmd_counter,
                                work->cmd.u.get.block_id, 0);
        break;
    case 0xa:
        MultiCopy_Int(st, &work->cmd, sizeof(WBTCommand));
        work->cmd.my_cmd_counter = t;
        ret = func_02064a94(q, work->cmd.target_bmp, work->cmd.my_cmd_counter,
                            work->cmd.u.user_data.data, work->cmd.u.user_data.size);
        break;
    }
    ((WBTCommand *)((unsigned int)&work->queued_cmd))->command = 0;
    return ret;
}
