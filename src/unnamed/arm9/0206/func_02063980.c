#include "types.h"
#include "private/arm9_wbt.h"

extern int func_02065bc0(void);
extern int func_02065bd0(void);
extern int func_02064f54(u8 *dst, u16 a1, u8 a2, int a3, int a4);
extern int func_0206470c(WBTPacketBitmap *pkt_bmp);
extern int func_02064bac(u8 *dst, u16 a1, u8 a2, u32 a3);
extern int func_02064e18(u8 *dst, u16 a1, u8 a2, u32 a3, u32 a4);
extern int func_02064cd8(u8 *dst, u16 a1, u8 a2, u32 a3, u32 a4);
extern int func_02064a94(u8 *dst, u16 a1, u8 a2, const void *a3, u8 a4);

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
