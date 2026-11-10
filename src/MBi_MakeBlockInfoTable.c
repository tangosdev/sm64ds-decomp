#include "private/arm9_a9db8_mb_parent.h"
extern int IsAbleToLoad(unsigned int idx, unsigned int addr, unsigned int size);
extern char *data_020a9db8;

/* NitroSDK MBi_MakeBlockInfoTable(table, mdfi): lay the three segments out as
 * consecutive mBlockSize-byte blocks, refusing a segment IsAbleToLoad
 * (MBi_IsAbleToRecv) says the child cannot receive. */
int MBi_MakeBlockInfoTable(int *c, char *list) {
    MB_BlockInfoTable *table = (MB_BlockInfoTable *)c;
    MbDownloadFileInfoHeader *mdfi = (MbDownloadFileInfoHeader *)list;
    unsigned char idx;
    unsigned short *blockHead;
    int sum;
    int r;

    /* Held in a local: indexing table->block_head directly grows the loop. */
    blockHead = table->block_head;

    sum = 0;
    if (list == 0)
        return 0;

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

        r = IsAbleToLoad(idx, addr, esize);
        if (r == 0)
            return 0;

        if (idx < 2) {
            blockHead[idx + 1] = sb;
        } else {
            table->block_num = sb;
        }
        idx = idx + 1;
    } while (idx < 3);

    return 1;
}
