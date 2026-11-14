#include "types.h"
#include "private/arm9_wbt.h"
#include "nitro/mi/memory.h"
extern int _Z13func_02065af0Pi(int *thiz);

/* NitroSDK WBT_GetBlockInfo: WBT_GetBlock's REQ_GET_BLOCK built as
 * REQ_GET_BLOCKINFO. Entry index of the peers' block list is fetched
 * into the per-aid buffers in recv_buf_table, each one WBTBlockInfo
 * (recv_data_size); only the receive-buffer table is filled. */
void WBT_GetBlockInfo(u16 target_bmp, int index, const void *recv_buf_table, WBTCallback callback) {
    WBTCommand cmd;
    cmd.command = WBT_CMD_REQ_GET_BLOCKINFO;
    cmd.target_bmp = target_bmp;
    cmd.callback = callback;
    cmd.u.get.block_id = index;
    cmd.u.get.recv_data_size = sizeof(WBTBlockInfo);
    MI_CpuCopy32(recv_buf_table, cmd.u.get.recv_buf_table, sizeof(cmd.u.get.recv_buf_table));
    _Z13func_02065af0Pi((int *)&cmd);
}
