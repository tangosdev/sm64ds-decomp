#include "private/arm9_a1fc0_file_share.h"

/* The parent's ready-list search: the first ready node (mState 2) whose
 * registered region starts at offset (mRegion's first word), else 0. */
FileShareNode *func_02041c2c(FileShareParentArea *area, int offset) {
    FileShareNode *node = area->mReadyList;
    if (node == 0)
        return node;
    do {
        if (node->mState == 2 && *(int *)node->mRegion == offset)
            return node;
        node = node->mNext;
    } while (node != 0);
    return node;
}
