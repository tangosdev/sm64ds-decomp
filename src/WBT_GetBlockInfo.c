#include "types.h"
#include "private/arm9_wbt.h"
extern void MIi_CpuCopy32(const void *src, void *dst, s32 size); /* 0x0205a490 */
extern int func_02065af0(int *thiz);

/* NitroSDK WBT_GetBlockInfo: WBT_GetBlock's REQ_GET_BLOCK built as
 * REQ_GET_BLOCKINFO (6). Entry arg1 of the peers' block list is fetched
 * into the per-aid buffers in src, each 0x28 bytes (recv_data_size); only
 * the receive-buffer table is filled. */
void WBT_GetBlockInfo(u16 type, u32 arg1, const void *src, u32 arg3) {
    WBTCommand cmd;
    cmd.command = 6;
    cmd.target_bmp = type;
    cmd.callback = (void (*)(char *))arg3;
    cmd.u.get.block_id = arg1;
    cmd.u.get.recv_data_size = 0x28;
    MIi_CpuCopy32(src, cmd.u.get.recv_buf_table, 0x40);
    func_02065af0((int *)&cmd);
}
