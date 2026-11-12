#include "private/arm9_a1fc0_file_share.h"
#include "nitro/fs/file.h"

/* Find the ready node whose file covers [offset, offset + size); 0 if none.
   func_020412f0 serves an open request from it. */
FileShareNode *func_02041ce0(FileShareParentArea *area, int offset, int size) {
    FileShareNode *node = area->mReadyList;
    if (node == 0)
        return node;
    do {
        if (node->mState == 2 && offset == node->mFile.prop.file.top &&
            size == FS_GetLength(&node->mFile))
            return node;
        node = node->mNext;
    } while (node != 0);
    return node;
}
