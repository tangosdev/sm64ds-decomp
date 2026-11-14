#include "types.h"
#include "private/arm9_wbt.h"
#include "nitro/mi/memory.h"
extern int _Z13func_02065af0Pi(int *thiz);

/* NitroSDK WBT_GetBlock: queue a REQ_GET_BLOCK command asking the peers
 * in target_bmp for block block_id, recv_size bytes, into the per-aid
 * receive buffers and packet bitmaps of the two 16-entry tables. */
void WBT_GetBlock(u16 target_bmp, u32 block_id, const void *recv_buf_table, u32 recv_size,
                  const void *pkt_bmp_table, WBTCallback callback) {
    WBTCommand cmd;
    cmd.command = WBT_CMD_REQ_GET_BLOCK;
    cmd.target_bmp = target_bmp;
    cmd.callback = callback;
    cmd.u.get.block_id = block_id;
    cmd.u.get.recv_data_size = recv_size;
    MI_CpuCopy32(recv_buf_table, cmd.u.get.recv_buf_table, sizeof(cmd.u.get.recv_buf_table));
    MI_CpuCopy32(pkt_bmp_table, cmd.u.get.pkt_bmp_table, sizeof(cmd.u.get.pkt_bmp_table));
    _Z13func_02065af0Pi((int *)&cmd);
}
