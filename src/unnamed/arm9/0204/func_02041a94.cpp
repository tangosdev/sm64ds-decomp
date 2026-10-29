//cpp
#include "private/arm9_a1fc0_file_share.h"
#include "IRQ.h"

extern "C" {

/* Moves node off the list headed by *from onto the front of the list headed
 * by *to, with interrupts off. The parent's three node lists (free, ready,
 * busy) change only through here: func_02041b60, func_02041b24,
 * func_02041ae8 and func_02041a94 are its four moves.
 *
 * The search never advances: the loop either unlinks the node from behind
 * the head or breaks. A node third in its list is left where it is, and
 * nothing is moved. That is the ROM's code (0x02041bf4..0x02041c14), not a
 * decompilation slip; with a pool of three nodes, a list can be three long. */
void func_02041bbc(FileShareNode **from, FileShareNode **to, FileShareNode *node) {
    unsigned int saved = IRQ::Disable();
    FileShareNode *cur = *from;
    if (cur == node) {
        *from = node->mNext;
        node->mNext = *to;
        *to = node;
    } else {
        while (cur != 0) {
            if (cur->mNext != node)
                break;
            cur->mNext = node->mNext;
            node->mNext = *to;
            *to = node;
            break;
        }
    }
    IRQ::Restore(saved);
}

/* free -> busy: takes the free list's head to be loaded (state 1) and
 * counts it in mBusyCount. */
FileShareNode *func_02041b60(FileShareParentArea *area) {
    FileShareNode *node;
    node = area->mFreeList;
    func_02041bbc(&area->mFreeList, &area->mBusyList, node);
    node->mState = 1;
    area->mBusyCount += 1;
    return node;
}

/* busy -> ready: the loader has read the region's first pages and
 * func_02041d28 has registered it (state 2). */
void func_02041b24(FileShareParentArea *area, FileShareNode *node) {
    func_02041bbc(&area->mBusyList, &area->mReadyList, node);
    node->mState = 2;
}

/* ready -> busy: no child reads the node's region any more, so the loader
 * closes its file (state 3, releasing). */
void func_02041ae8(FileShareParentArea *area, FileShareNode *node) {
    func_02041bbc(&area->mReadyList, &area->mBusyList, node);
    node->mState = 3;
}

/* busy -> free: the loader has closed the node's file (state 0), and the
 * node is no longer counted in mBusyCount. */
void func_02041a94(FileShareParentArea *area, FileShareNode *node) {
    func_02041bbc(&area->mBusyList, &area->mFreeList, node);
    node->mState = 0;
    area->mBusyCount -= 1;
}
}
