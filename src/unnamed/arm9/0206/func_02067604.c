extern int func_0206736c(unsigned int idx, unsigned int addr, unsigned int size);
extern void *data_020a9db8;

/* NitroSDK MbSegmentInfo: one row of a download file's segment table, as
 * func_020676e0 fills it (ARM9 static, ARM7 static, ROM header). */
typedef struct MbSegmentInfo {
    unsigned int recv_addr;  /* 0x00 -- where the child receives it: load_addr, or a
                                staging buffer for an ARM7 segment */
    unsigned int load_addr;  /* 0x04 -- where it finally runs */
    unsigned int size;       /* 0x08 */
    unsigned int target : 1; /* 0x0c -- set for the ARM7 segment */
    unsigned int rsv : 31;
} MbSegmentInfo;

/* NitroSDK MbDownloadFileInfoHeader, the start of MbParentFile.mHeader
 * (func_020678a4 builds it). */
typedef struct MbDownloadFileInfoHeader {
    unsigned int arm9EntryAddr; /* 0x00 */
    unsigned int arm7EntryAddr; /* 0x04 */
    unsigned int padding;       /* 0x08 */
    MbSegmentInfo seg[3];       /* 0x0c */
} MbDownloadFileInfoHeader;

/* NitroSDK MB_BlockInfoTable, MbParentFile.mBlockTable: each segment's byte
 * offset and first block number in the stream, and the total block count. */
typedef struct MB_BlockInfoTable {
    unsigned int seg_head_offset[3]; /* 0x00 */
    unsigned short block_head[3];    /* 0x0c */
    unsigned short block_num;        /* 0x12 */
} MB_BlockInfoTable;

/* File-local mirror of include/private/arm9_a9db8_mb_parent.h's
 * MbParentWork, the one field read here. */
typedef struct MbParentWork {
    unsigned char pad_0000[0x1318];
    unsigned int mBlockSize; /* 0x1318 */
} MbParentWork;

/* NitroSDK MBi_MakeBlockInfoTable(table, mdfi): lay the three segments out as
 * consecutive mBlockSize-byte blocks, refusing a segment func_0206736c
 * (MBi_IsAbleToRecv) says the child cannot receive. */
int func_02067604(int *c, char *list)
{
    MB_BlockInfoTable *table = (MB_BlockInfoTable *)c;
    MbDownloadFileInfoHeader *mdfi = (MbDownloadFileInfoHeader *)list;
    unsigned char idx;
    unsigned short *blockHead;
    int sum;
    int r;

    /* Held in a local: indexing table->block_head directly grows the loop. */
    blockHead = table->block_head;

    sum = 0;
    if (list == 0) return 0;

    idx = 0;
    do {
        table->seg_head_offset[idx] = sum;
        sum += mdfi->seg[idx].size;
        idx = idx + 1;
    } while (idx < 3);

    idx = 0;
    blockHead[0] = 0;
    do {
        MbSegmentInfo *e = &mdfi->seg[idx];
        unsigned int divisor = ((MbParentWork *)data_020a9db8)->mBlockSize;
        unsigned int esize = e->size;
        unsigned int n = (esize + divisor - 1) / divisor;
        unsigned short sb = (unsigned short)(blockHead[idx] + (unsigned short)n);
        unsigned int addr = e->load_addr;

        r = func_0206736c(idx, addr, esize);
        if (r == 0) return 0;

        if (idx < 2) {
            blockHead[idx + 1] = sb;
        } else {
            table->block_num = sb;
        }
        idx = idx + 1;
    } while (idx < 3);

    return 1;
}
