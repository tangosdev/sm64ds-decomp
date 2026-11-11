//cpp
#include "types.h"
#include "private/arm9_a1fc0_file_share.h"
#include "private/arm9_wbt.h"
#include "nitro/os/cache.h"
#include "private/fs_views.h"

/* The FSFile a child read is for (FileShareWork.mReadRequest, set by
 * func_02041a00). */
typedef struct FSFile FSFile;

extern "C" {

extern void func_02041a00(FSFile *file);
extern int func_020423dc(int *table);
extern void FS_NotifyArchiveAsyncEnd(void *self, int arg);
extern void func_02042160(void *arg);
extern void func_02040f30(void *arg);
extern char data_020a2400[];
void func_02040f30(void *arg) {
    WBTCommand *cmd = (WBTCommand *)arg;
    int aid = WBT_AidbitmapToAid(cmd->peer_bmp);
    FileShareChildArea *child = (FileShareChildArea *)data_020a2400;
    switch (cmd->command) {
    case 2:
        if (cmd->target_bmp != 0) {
            return;
        }
        data_020a1fc0.mParentPacketSize = cmd->u.sync.peer_packet_size + 0xe;
        data_020a1fc0.mChildPacketSize = cmd->u.sync.my_packet_size + 0xe;
        if (data_020a1fc0.mState != 2) {
            WBT_GetBlockInfo(1, 0, child->mBlockInfoTable, func_02040f30);
            return;
        }
        if (data_020a1fc0.mReadRequest != 0) {
            func_02041a00(data_020a1fc0.mReadRequest);
        }
        return;

    case 6:
        child->mRecvBufTable[aid] = data_020a1fc0.mTable;
        WBT_GetBlock(1, 0x20000, child->mRecvBufTable, data_020a1fc0.mTableSize,
                     child->mPacketBitmapTable, func_02040f30);
        return;

    case 4:
        if (cmd->target_bmp != 0) {
            return;
        }
        if (data_020a1fc0.mState != 2) {
            DC_FlushRange(data_020a1fc0.mTable, data_020a1fc0.mTableSize);
            func_020423dc((int *)data_020a1fc0.mTable);
            data_020a1fc0.mState = 2;
            if (data_020a1fc0.mStateCallback != 0) {
                data_020a1fc0.mStateCallback(0);
            }
            return;
        }
        {
            FileShareMsg msg;
            u32 region = child->mReadRegion;
            int packetSize;
            msg.mKind = 2;
            msg.mFlag = 1;
            packetSize = data_020a1fc0.mParentPacketSize;
            msg.mFileId = region;
            msg.mPacketSizeHigh = (u8)(packetSize >> 8);
            msg.mPacketSizeLow = (u8)packetSize;
            WBT_PutUserData(1, &msg, 9, func_02042160);
        }
        return;

    case 0xc: {
        FileShareMsg *m;
        if (cmd->event != 0xa) {
            return;
        }
        m = (FileShareMsg *)cmd->u.user_data.data;
        switch (m->mKind) {
        case 1:
            if (m->mFlag == 0) {
                WBT_RequestSync(1, func_02040f30);
                return;
            }
            child->mReadRegion = m->mFileId;
            child->mRecvBufTable[aid] = data_020a1fc0.mReadRequest->arg.readfile.dst;
            WBT_GetBlock(1, child->mReadRegion, child->mRecvBufTable,
                         data_020a1fc0.mReadRequest->arg.readfile.len, child->mPacketBitmapTable,
                         func_02040f30);
            return;

        case 3: {
            FSFile *p = data_020a1fc0.mReadRequest;
            void *arc = p->arc;
            data_020a1fc0.mReadRequest = 0;
            p->prop.file.pos += p->arg.readfile.len;
            FS_NotifyArchiveAsyncEnd(arc, 0);
            return;
        }
        }

        return;
    }
    }
}
}
