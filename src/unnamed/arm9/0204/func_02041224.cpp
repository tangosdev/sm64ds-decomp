//cpp
#include "private/arm9_a1fc0_file_share.h"
#include "IRQ.h"

extern "C" {

typedef int (*FnT)(int a, int b, int c);

extern char data_020a1fc0[];
extern unsigned int __aeabi_uidiv(unsigned int n, unsigned int d);

/* Grow the child's receive work (WBT's packet bitmap) to cover a read of n
 * bytes, or with n < 0 reallocate it at the current mMaxReadSize. */
void func_02041224(char *thiz, int n)
{
    FileShareChildArea *child = (FileShareChildArea *)thiz;
    int r5;
    int r6;
    unsigned int saved;
    unsigned int q;
    char *g; /* char *, cast at each use: a FileShareWork * local is 12 bytes longer */
    if (n < 0) {
        n = child->mMaxReadSize;
        child->mMaxReadSize = 0;
    }
    if ((unsigned int)child->mMaxReadSize >= (unsigned int)n)
        return;
    child->mMaxReadSize = n;
    r6 = child->mRecvWork;
    if (r6 != 0) {
        g = data_020a1fc0;
        saved = IRQ::Disable();
        ((FnT)((FileShareWork *)g)->mAllocFunc)(((FileShareWork *)g)->mAllocArg, 0, r6);
        IRQ::Restore(saved);
    }
    {
        int dm = FILE_SHARE_WORK->mParentPacketSize - 0xe;
        q = __aeabi_uidiv(child->mMaxReadSize + dm - 1, dm);
    }
    r5 = ((q + 0x1f) >> 2) << 4;
    g = data_020a1fc0;
    saved = IRQ::Disable();
    r5 = ((FnT)((FileShareWork *)g)->mAllocFunc)(((FileShareWork *)g)->mAllocArg, r5, 0);
    IRQ::Restore(saved);
    child->mRecvWork = r5;
    child->mPacketBitmapTable[0] = child->mRecvWork;
}
}
