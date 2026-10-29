//cpp
#include "types.h"
#include "private/arm9_a1fc0_file_share.h"
#include "IRQ.h"

extern "C" {

extern void func_0205cd5c(void *arg0, int arg1);
extern void func_020421b4(void);
extern void func_02065170(void);

extern int data_020a0f84;
extern char data_020a1fc0[];

/* mAllocFunc: (arg, size, 0) allocates, (arg, 0, ptr) frees. */
typedef void (*CallbackFn)(void *ctx, int zero, int val);

/* The FSFile a child read is in flight on (mReadRequest); only its archive is
 * read here (NitroSDK FSFile.arc). */
typedef struct FSFile {
    char pad_00[8];
    void *arc; /* 0x08 */
} FSFile;

/* Shut the file share down: free the card tables (mTable), and as a child
 * cancel the read in flight and free the receive work (mRecvWork). The
 * mAllocFunc/mAllocArg and mIsParent reads go through FILE_SHARE_WORK, not
 * work: the ROM reloads the block's address for them (through work the
 * function is 12 bytes shorter). */
void func_02040aa4(void) {
    FileShareWork *work = FILE_SHARE_WORK;
    u32 saved0 = IRQ::Disable();

    if (data_020a0f84 != 0) {
        int cb1 = work->mTable;

        if (cb1 != 0) {
            if (cb1 != 0) {
                u32 saved1 = IRQ::Disable();
                void *ctxArg = (void *)FILE_SHARE_WORK->mAllocArg;
                CallbackFn fn = (CallbackFn)FILE_SHARE_WORK->mAllocFunc;
                fn(ctxArg, 0, cb1);
                IRQ::Restore(saved1);
            }
            *(volatile int *)&work->mTable = 0;
            *(volatile int *)&work->mTableSize = 0;
        }

        work->mState = 0;

        if (FILE_SHARE_WORK->mIsParent == 0) {
            FSFile *p = (FSFile *)work->mReadRequest;
            FileShareChildArea *child = (FileShareChildArea *)work->mArea;
            if (p != 0) {
                void *arg = p->arc;
                work->mReadRequest = 0;
                func_0205cd5c(arg, 5);
            }

            {
                int cb2 = child->mRecvWork;
                if (cb2 != 0) {
                    u32 saved2 = IRQ::Disable();
                    void *ctxArg2 = (void *)FILE_SHARE_WORK->mAllocArg;
                    CallbackFn fn2 = (CallbackFn)FILE_SHARE_WORK->mAllocFunc;
                    fn2(ctxArg2, 0, cb2);
                    IRQ::Restore(saved2);
                }
            }
        } else {
            func_020421b4();
        }

        func_02065170();
        data_020a0f84 = 0;
    }

    IRQ::Restore(saved0);
}
}
