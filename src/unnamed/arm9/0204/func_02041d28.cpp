//cpp
#include "private/arm9_a1fc0_file_share.h"
#include "IRQ.h"

extern "C" {

extern int WBT_RegisterBlock(void *node, int a1, void *a2, int a3, int arg5, unsigned short arg6);
extern void func_02041b24(FileShareParentArea *area, FileShareNode *node);

extern char data_02082158[];

/* A node has finished loading (func_02042254). The children that asked for it
   while it was loading become owed a reply. If no child holds it yet, it is
   registered with WBT_RegisterBlock (with the first page as data when the region
   fits in 0x400 bytes) and moved to the ready list by func_02041b24. Each
   waiting child then takes a reference and gets the node's file id in its
   reply slot. Only while mState is set. The third word is
   unused: func_02042254 passes 1 (reloaded from its frame into r2). */
void func_02041d28(char *r7, FileShareNode *node, int) {
    unsigned int saved = IRQ::Disable();
    int mask;

    if (data_020a1fc0.mState != 0) {
        int bit;
        int *q = &((FileShareParentArea *)r7)->mReplyPending;
        mask = node->mWaitingChildren;
        node->mWaitingChildren = 0;
        *q |= mask;
        if (node->mRefCount <= 0) {
            unsigned int diff = FILE_SHARE_EXTENT(node)->bottom - FILE_SHARE_EXTENT(node)->top;
            int fileId = node->mFileId;
            WBT_RegisterBlock(node->mRegion, fileId, data_02082158,
                              (diff <= 0x400) ? (int)node->mPages[0] : 0, diff, 0);
            func_02041b24((FileShareParentArea *)r7, node);
        }
        for (bit = 0;; bit++) {
            if ((1 << bit) > mask) {
                break;
            }
            if ((1 << bit) & mask) {
                node->mRefCount++;
                ((FileShareMsg *)r7)->mFileId = node->mFileId;
            }
            r7 += 0xc;
        }
    }
    IRQ::Restore(saved);
}
}
