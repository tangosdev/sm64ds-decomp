//cpp
#include "types.h"
#include "private/arm9_a1fc0_file_share.h"
#include "IRQ.h"
#include "private/fs_views.h"

/* The FSFile a child read is in flight on (mReadRequest); only its archive is
 * read here. */
typedef struct FSFile FSFile;

extern "C" {

extern void FS_NotifyArchiveAsyncEnd(void *arc, int result);
extern void func_020421b4(void);
extern void WBT_End(void);

extern int data_020a0f84;

/* mAllocFunc: (arg, size, 0) allocates, (arg, 0, ptr) frees. */
typedef void (*CallbackFn)(void *ctx, int zero, int val);

/* Shut the file share down: free the card tables (mTable), and as a child
 * cancel the read in flight and free the receive work (mRecvWork). The
 * mAllocFunc/mAllocArg and mIsParent reads go through data_020a1fc0, not
 * work: the ROM reloads the block's address for them (through work the
 * function is 12 bytes shorter). */
void func_02040aa4(void) {
    FileShareWork *work = &data_020a1fc0;
    u32 saved0 = IRQ::Disable();

    if (data_020a0f84 != 0) {
        int table = work->mTable;

        if (table != 0) {
            if (table != 0) {
                u32 saved1 = IRQ::Disable();
                void *ctxArg = (void *)data_020a1fc0.mAllocArg;
                CallbackFn fn = (CallbackFn)data_020a1fc0.mAllocFunc;
                fn(ctxArg, 0, table);
                IRQ::Restore(saved1);
            }
            work->mTable = 0;
            work->mTableSize = 0;
        }

        work->mState = 0;

        if (data_020a1fc0.mIsParent == 0) {
            FSFile *p = (FSFile *)work->mReadRequest;
            FileShareChildArea *child = (FileShareChildArea *)work->mArea;
            if (p != 0) {
                void *arg = p->arc;
                work->mReadRequest = 0;
                FS_NotifyArchiveAsyncEnd(arg, 5);
            }

            {
                int recvWork = child->mRecvWork;
                if (recvWork != 0) {
                    u32 saved2 = IRQ::Disable();
                    void *ctxArg2 = (void *)data_020a1fc0.mAllocArg;
                    CallbackFn fn2 = (CallbackFn)data_020a1fc0.mAllocFunc;
                    fn2(ctxArg2, 0, recvWork);
                    IRQ::Restore(saved2);
                }
            }
        } else {
            func_020421b4();
        }

        WBT_End();
        data_020a0f84 = 0;
    }

    IRQ::Restore(saved0);
}
}
