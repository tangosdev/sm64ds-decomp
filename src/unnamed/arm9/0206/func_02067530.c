#include "types.h"
#include "private/arm9_a9db8_mb_parent.h"
extern char* data_020a9db8;

/* NitroSDK MBi_get_blockinfo(bi, table, block, dlinfo): find the segment
 * holding block number val (the last segment whose first block is <= it),
 * then give the block's receive address, size (the rest of the segment,
 * capped at mBlockSize), source offset and segment number. func_0206610c
 * calls it for the block it is about to send.
 *
 * The views are func_02067604's MbSegmentInfo, MbDownloadFileInfoHeader
 * and MB_BlockInfoTable. The result keeps func_0206610c's struct Out tag
 * (its declaration names the parameter type) with the SDK's MB_BlockInfo
 * field names. */
typedef struct MbSegmentInfo {
    unsigned int recv_addr;  /* 0x00 */
    unsigned int load_addr;  /* 0x04 */
    unsigned int size;       /* 0x08 */
    unsigned int target : 1; /* 0x0c */
    unsigned int rsv : 31;
} MbSegmentInfo;

typedef struct MbDownloadFileInfoHeader {
    unsigned int arm9EntryAddr; /* 0x00 */
    unsigned int arm7EntryAddr; /* 0x04 */
    unsigned int padding;       /* 0x08 */
    MbSegmentInfo seg[3];       /* 0x0c */
} MbDownloadFileInfoHeader;

typedef struct MB_BlockInfoTable {
    unsigned int seg_head_offset[3]; /* 0x00 */
    unsigned short block_head[3];    /* 0x0c */
    unsigned short block_num;        /* 0x12 */
} MB_BlockInfoTable;

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
