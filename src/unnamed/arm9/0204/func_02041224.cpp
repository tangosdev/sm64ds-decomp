//cpp
#include "private/arm9_a1fc0_file_share.h"
#include "IRQ.h"

extern "C" {

typedef int (*FnT)(int a, int b, int c);

/* Grow the child's receive work (WBT's packet bitmap) to cover a read of n
 * bytes, or with n < 0 reallocate it at the current mMaxReadSize. */
void func_02041224(char *thiz, int n)
{
    FileShareChildArea *child = (FileShareChildArea *)thiz;
    int work;
    int oldWork;
    unsigned int saved;
    unsigned int q;
    if (n < 0) {
        n = child->mMaxReadSize;
        child->mMaxReadSize = 0;
    }
    if ((unsigned int)child->mMaxReadSize >= (unsigned int)n)
        return;
    child->mMaxReadSize = n;
    oldWork = child->mRecvWork;
    if (oldWork != 0) {
        saved = IRQ::Disable();
        ((FnT)data_020a1fc0.mAllocFunc)(data_020a1fc0.mAllocArg, 0, oldWork);
        IRQ::Restore(saved);
    }
    {
        u32 dm = data_020a1fc0.mParentPacketSize - 0xe;
        q = (child->mMaxReadSize + dm - 1) / dm;
    }
    work = ((q + 0x1f) >> 2) << 4; /* the bitmap's size, then the work itself */
    saved = IRQ::Disable();
    work = ((FnT)data_020a1fc0.mAllocFunc)(data_020a1fc0.mAllocArg, work, 0);
    IRQ::Restore(saved);
    child->mRecvWork = work;
    child->mPacketBitmapTable[0] = child->mRecvWork;
}
}
