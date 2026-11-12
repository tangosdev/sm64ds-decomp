#include "types.h"
#include "private/arm9_wbt.h"
#include "nitro/mi/memory.h"
extern int func_02065af0(int *thiz);

/* NitroSDK WBT_GetBlock: queue a REQ_GET_BLOCK command (4) asking the peers
 * in target_bmp for block block_id, recv_size bytes, into the per-aid
 * receive buffers and packet bitmaps of the two 16-entry tables. */
void WBT_GetBlock(u16 target_bmp, u32 block_id, const void *recv_buf_table, u32 recv_size,
                  const void *pkt_bmp_table, void (*callback)(void *)) {
    WBTCommand cmd;
    cmd.command = 4;
    cmd.target_bmp = target_bmp;
    cmd.callback = (void (*)(char *))callback;
    cmd.u.get.block_id = block_id;
    cmd.u.get.recv_data_size = recv_size;
    MIi_CpuCopy32(recv_buf_table, cmd.u.get.recv_buf_table, 0x40);
    MIi_CpuCopy32(pkt_bmp_table, cmd.u.get.pkt_bmp_table, 0x40);
    func_02065af0((int *)&cmd);
}
