#include "types.h"
#include "private/arm9_wbt.h"
extern void MultiCopy_Int(const void *src, void *dst, s32 size); /* 0x0205a490 */
extern int func_02065af0(int *thiz);

/* NitroSDK WBT_GetBlock: queue a REQ_GET_BLOCK command (4) asking the peers
 * in type (target_bmp) for block arg1, recv_data_size arg3 bytes, into the
 * per-aid receive buffers and packet bitmaps of the two 16-entry tables. */
void func_02065730(u16 type, u32 arg1, const void *src1, u32 arg3, const void *src2, u32 extra)
{
    WBTCommand cmd;
    cmd.command = 4;
    cmd.target_bmp = type;
    cmd.callback = (void (*)(char *))extra;
    cmd.u.get.block_id = arg1;
    cmd.u.get.recv_data_size = arg3;
    MultiCopy_Int(src1, cmd.u.get.recv_buf_table, 0x40);
    MultiCopy_Int(src2, cmd.u.get.pkt_bmp_table, 0x40);
    func_02065af0((int *)&cmd);
}
