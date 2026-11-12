#include "private/arm9_a1fc0_file_share.h"

/* The parent's ready-list search: the first ready node (mState 2) whose
 * registered block is blockId (the WBT block id WBT_RegisterBlock stored in
 * mRegion), else 0. */
FileShareNode *func_02041c2c(FileShareParentArea *area, int blockId) {
    FileShareNode *node = area->mReadyList;
    if (node == 0)
        return node;
    do {
        if (node->mState == 2 && node->mRegion.data_info.id == blockId)
            return node;
        node = node->mNext;
    } while (node != 0);
    return node;
}
