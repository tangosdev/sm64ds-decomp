//cpp
#include "types.h"
#include "private/file_share_alloc.h"
#include "nitro/os/system.h"
#include "nitro/fs/archive.h"

/* The FSFile a child read is in flight on (mReadRequest); only its archive is
 * read here. */

extern "C" {

extern void func_020421b4(void);
extern void WBT_End(void);

extern int data_020a0f84;

/* Shut the file share down: free the card tables (mTable), and as a child
 * cancel the read in flight and free the receive work (mRecvWork). The
 * mIsParent read goes through data_020a1fc0, not work, as FileShareFree's
 * allocator reads do: the ROM reloads the block's address for it (through
 * work the function comes out shorter). */
void func_02040aa4(void) {
    FileShareWork *work = &data_020a1fc0;
    OSIntrMode saved0 = OS_DisableInterrupts();

    if (data_020a0f84 != 0) {
        void *table = work->mTable;

        if (table != 0) {
            FileShareFree(table);
            work->mTable = 0;
            work->mTableSize = 0;
        }

        work->mState = 0;

        if (data_020a1fc0.mIsParent == 0) {
            FSFile *p = work->mReadRequest;
            FileShareChildArea *child = (FileShareChildArea *)work->mArea;
            if (p != 0) {
                FSArchive *arc = p->arc;
                work->mReadRequest = 0;
                FS_NotifyArchiveAsyncEnd(arc, FS_RESULT_ERROR);
            }
            FileShareFree(child->mRecvWork);
        } else {
            func_020421b4();
        }

        WBT_End();
        data_020a0f84 = 0;
    }

    OS_RestoreInterrupts(saved0);
}
}
