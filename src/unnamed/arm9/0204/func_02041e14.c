#include "types.h"
#include "private/arm9_a1fc0_file_share.h"
#include "private/arm9_wbt.h"

extern int WBT_SetPacketSize(int a, int b);
extern void func_02042160(void *arg);

void func_02041e14(void)
{
    FileShareParentArea *area = (FileShareParentArea *)FILE_SHARE_WORK->mArea;
    FileShareMsg resizeMsg;
    FileShareMsg replyMsg;

    if (data_020a1fc0.mIsParent == 0) {
        return;
    }
    if (area->mReplySending != 0) {
        return;
    }

    area->mReplyPending &= area->mConnectedBitmap;
    area->mReplyGroup &= area->mConnectedBitmap;
    area->mOpenChildren &= area->mConnectedBitmap;
    area->mResizeChildren &= area->mConnectedBitmap;

    if (area->mResizePending != 0 && area->mBusyCount == 0) {
        int newSize = area->mNewPacketSize;
        int childSize = data_020a1fc0.mChildPacketSize;
        int children;

        area->mResizePending = 0;
        data_020a1fc0.mParentPacketSize = newSize;
        WBT_SetPacketSize(newSize, childSize);

        children = area->mResizeChildren;
        if (children == 0) {
            return;
        }
        resizeMsg.mKind = 1;
        resizeMsg.mFlag = 0;
        resizeMsg.mFileId = 0;
        resizeMsg.mPacketSizeHigh = (u8)(data_020a1fc0.mParentPacketSize >> 8);
        resizeMsg.mPacketSizeLow = (u8)data_020a1fc0.mParentPacketSize;
        WBT_PutUserData(children, &resizeMsg, 9, func_02042160);
        area->mReplySending = 1;
        area->mResizeChildren = 0;
    } else {
        int mask;
        FileShareMsg *first;
        int grouped;
        int i;
        int group;
        int bit;
        FileShareMsg *msg;
        u8 kind;
        int packetSize;
        int fileId;

        mask = area->mReplyPending;
        if (mask == 0) {
            return;
        }
        group = area->mReplyGroup;
        first = 0;
        grouped = (group != 0 && group == (group & mask));
        if (grouped) {
            mask = group;
        } else {
            mask &= ~group;
        }

        msg = (FileShareMsg *)area->mReplies;
        i = 0;
        for (;;) {
            bit = 1 << i;
            if (bit > mask) {
                break;
            }
            if (bit & mask) {
                if (first == 0) {
                    first = msg;
                } else if (first->mKind != msg->mKind || first->mFileId != msg->mFileId) {
                    mask &= ~bit;
                }
            }
            msg++;
            i++;
        }

        if (grouped && mask != group) {
            area->mReplyGroup = 0;
        }
        if (first == 0) {
            return;
        }

        switch (first->mKind) {
        case 0:
            first->mKind = 1;
            break;
        case 2:
            first->mKind = 3;
            area->mOpenChildren &= ~mask;
            break;
        }

        kind = first->mKind;
        fileId = first->mFileId;
        packetSize = data_020a1fc0.mParentPacketSize;
        replyMsg.mKind = kind;
        replyMsg.mFlag = 1;
        replyMsg.mFileId = fileId;
        replyMsg.mPacketSizeHigh = (u8)(packetSize >> 8);
        replyMsg.mPacketSizeLow = (u8)packetSize;
        WBT_PutUserData(mask, &replyMsg, 9, func_02042160);
        area->mReplySending = 1;
        area->mReplyPending &= ~mask;
    }
}
