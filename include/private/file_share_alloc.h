#ifndef FILE_SHARE_ALLOC_H
#define FILE_SHARE_ALLOC_H

#include "private/arm9_a1fc0_file_share.h"
#include "nitro/os/system.h"

/* The file share's allocator calls, each made with interrupts off:
 * mAllocFunc(arg, size, 0) allocates, mAllocFunc(arg, 0, ptr) frees. Both
 * read the work block through data_020a1fc0 itself, so a caller holding a
 * FileShareWork pointer still reloads the block's address for them.
 *
 * Only the files that call them include this header. A function body in a
 * header takes compiler @NNN numbers in every file that includes it. */
static inline void *FileShareAlloc(unsigned int size) {
    OSIntrMode saved = OS_DisableInterrupts();
    void *p = data_020a1fc0.mAllocFunc(data_020a1fc0.mAllocArg, size, 0);
    OS_RestoreInterrupts(saved);
    return p;
}

static inline void FileShareFree(void *p) {
    if (p != 0) {
        OSIntrMode saved = OS_DisableInterrupts();
        data_020a1fc0.mAllocFunc(data_020a1fc0.mAllocArg, 0, p);
        OS_RestoreInterrupts(saved);
    }
}

#endif
