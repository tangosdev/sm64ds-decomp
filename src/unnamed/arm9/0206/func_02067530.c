#include "types.h"
#include "private/arm9_a9db8_mb_parent.h"
extern char* data_020a9db8;

/* NitroSDK MBi_get_blockinfo(bi, table, block, dlinfo): find the segment
 * holding block number val (the last segment whose first block is <= it),
 * then give the block's receive address, size (the rest of the segment,
 * capped at mBlockSize), source offset and segment number. MBi_CommParentSendBlock
 * calls it for the block it is about to send.
 *
 * The views are MbSegmentInfo, MbDownloadFileInfoHeader and
 * MB_BlockInfoTable (private/arm9_a9db8_mb_parent.h). The result keeps
 * MBi_CommParentSendBlock's struct Out tag (its declaration names the
 * parameter type) with the SDK's MB_BlockInfo field names. */
struct Out {
    int child_recv_buff_addr; /* 0x0 */
    u32 size;                 /* 0x4 */
    int offset;               /* 0x8 */
    u8 segment_no;            /* 0xc */
};

int func_02067530(struct Out *o, char *in, u32 val, char *tbl) {
    MB_BlockInfoTable *table = (MB_BlockInfoTable *)in;
    MbDownloadFileInfoHeader *dlinfo = (MbDownloadFileInfoHeader *)tbl;
    int i;
    if (val >= table->block_num)
        return 0;
    i = 2;
    for (;;) {
        if (val >= table->block_head[i])
            break;
        i = (signed char)(i - 1);
        if (i < 0)
            break;
    }
    if (i < 0)
        return 0;
    {
        int span = ((MbParentWork *)data_020a9db8)->mBlockSize;
        int d = val - table->block_head[i];
        int rel = d * span;
        MbSegmentInfo *row = &dlinfo->seg[i];
        o->size = row->size - rel;
        {
            u32 lim = ((MbParentWork *)data_020a9db8)->mBlockSize;
            if (o->size > lim)
                o->size = lim;
        }
        o->offset = rel + table->seg_head_offset[i];
        o->child_recv_buff_addr = rel + row->recv_addr;
        o->segment_no = (u8)i;
    }
    return 1;
}
