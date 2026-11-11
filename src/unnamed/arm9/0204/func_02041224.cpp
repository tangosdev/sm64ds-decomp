//cpp
#include "private/file_share_alloc.h"

extern "C" {

/* Grow the child's receive work (WBT's packet bitmap) to cover a read of n
 * bytes, or with n < 0 reallocate it at the current mMaxReadSize. */
void func_02041224(char *thiz, int n)
{
    FileShareChildArea *child = (FileShareChildArea *)thiz;
    void *work;
    unsigned int size;
    if (n < 0) {
        n = child->mMaxReadSize;
        child->mMaxReadSize = 0;
    }
    if ((unsigned int)child->mMaxReadSize >= (unsigned int)n)
        return;
    child->mMaxReadSize = n;
    FileShareFree(child->mRecvWork);
    {
        u32 payload = data_020a1fc0.mParentPacketSize - 0xe;  /* bytes per packet */
        size = (child->mMaxReadSize + payload - 1) / payload; /* packets */
    }
    size = ((size + 0x1f) >> 2) << 4; /* packets, then the work's size in bytes */
    work = FileShareAlloc(size);
    child->mRecvWork = work;
    child->mPacketBitmapTable[0] = child->mRecvWork;
}
}
