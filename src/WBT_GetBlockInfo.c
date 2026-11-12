#include "types.h"
#include "private/arm9_wbt.h"
#include "nitro/mi/memory.h"
extern int func_02065af0(int *thiz);

/* NitroSDK WBT_GetBlockInfo: WBT_GetBlock's REQ_GET_BLOCK built as
 * REQ_GET_BLOCKINFO (6). Entry block_id of the peers' block list is fetched
 * into the per-aid buffers in recv_buf_table, each 0x28 bytes
 * (recv_data_size); only the receive-buffer table is filled. */
void WBT_GetBlockInfo(u16 target_bmp, u32 block_id, const void *recv_buf_table,
                      void (*callback)(void *)) {
    WBTCommand cmd;
    cmd.command = 6;
    cmd.target_bmp = target_bmp;
    cmd.callback = (void (*)(char *))callback;
    cmd.u.get.block_id = block_id;
    cmd.u.get.recv_data_size = 0x28;
    MIi_CpuCopy32(recv_buf_table, cmd.u.get.recv_buf_table, 0x40);
    func_02065af0((int *)&cmd);
}
