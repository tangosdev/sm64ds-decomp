#include "private/arm9_a1fc0_file_share.h"
extern FileShareNode *func_02041b60(FileShareParentArea *area);
extern void func_020423c8(void);

/* The parent's node for a child's request (offset, length): a loading node
 * already holding that extent, or a fresh one off the free list
 * (func_02041b60) with the extent stored and the loader thread woken
 * (func_020423c8). */
FileShareNode *func_02041c64(FileShareParentArea *area, int offset, int size) {
    FileShareNode *node;
    node = area->mBusyList;
    while (node != 0) {
        if (node->mState == 1 && node->mFileOffset == offset && node->mFileSize == size) {
            break;
        }
        node = node->mNext;
    }
    if (node == 0) {
        node = func_02041b60(area);
        node->mRefCount = 0;
        node->mFileOffset = offset;
        node->mFileSize = size;
        func_020423c8();
    }
    return node;
}
