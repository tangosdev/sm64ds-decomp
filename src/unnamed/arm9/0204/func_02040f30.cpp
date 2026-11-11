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

extern int WBT_AidbitmapToAid(int x);
extern void WBT_GetBlockInfo(u16 type, u32 arg1, const void *src, u32 arg3);
extern void func_02041a00(FSFile *file);
extern void WBT_GetBlock(u16 type, u32 arg1, const void *src1, u32 arg3, const void *src2,
                         u32 extra);
extern int func_020423dc(int *table);
extern int WBT_PutUserData(u16 arg0, const void *arg1, int arg2, u32 arg3);
extern void WBT_RequestSync(u16 id, void *obj);
extern void FS_NotifyArchiveAsyncEnd(void *self, int arg);
extern void func_02042160(void);
extern void func_02040f30(void *arg);
extern char data_020a2400[];
void func_02040f30(void *arg) {
    WBTCommand *cmdp = (WBTCommand *)arg;
    FileShareChildArea *child;
    int idx;
    int type;
    idx = WBT_AidbitmapToAid(cmdp->peer_bmp);
    type = cmdp->command;
    child = (FileShareChildArea *)data_020a2400;
    switch (type) {
    case 2:
        if (cmdp->target_bmp != 0) {
            return;
        }
        data_020a1fc0.mParentPacketSize = cmdp->u.sync.peer_packet_size + 0xe;
        data_020a1fc0.mChildPacketSize = cmdp->u.sync.my_packet_size + 0xe;
        if (data_020a1fc0.mState != 2) {
            WBT_GetBlockInfo((u16)1, 0, (const void *)child->mBlockInfoTable, (u32)func_02040f30);
            return;
        }
        {
            int request = data_020a1fc0.mReadRequest;
            if (request != 0) {
                func_02041a00((FSFile *)request);
            }
        }
        return;

    case 6:
        child->mRecvBufTable[idx] = data_020a1fc0.mTable;
        WBT_GetBlock((u16)1, 0x20000, (const void *)child->mRecvBufTable,
                     (u32)data_020a1fc0.mTableSize, (const void *)child->mPacketBitmapTable,
                     (u32)func_02040f30);
        return;

    case 4:
        if (cmdp->target_bmp != 0) {
            return;
        }
        if ((data_020a1fc0.mState != 2) != 0) {
            DC_FlushRange((void *)data_020a1fc0.mTable, (u32)data_020a1fc0.mTableSize);
            func_020423dc((int *)data_020a1fc0.mTable);
            {
                data_020a1fc0.mState = 2;
                {
                    int fn = data_020a1fc0.mStateCallback;
                    if (fn != 0) {
                        ((void (*)(int))fn)(0);
                    }
                }
            }
            return;
        }
        {
            u32 w4;
            int c;
            w4 = child->mReadRegion;
            FileShareMsg cmd;
            cmd.mKind = 2;
            cmd.mFlag = 1;
            c = data_020a1fc0.mParentPacketSize;
            cmd.mFileId = w4;
            cmd.mPacketSizeHigh = (u8)(c >> 8);
            cmd.mPacketSizeLow = (u8)c;
            WBT_PutUserData((u16)1, &cmd, 9, (u32)func_02042160);
        }
        return;

    case 0xc: {
        FileShareMsg *m;
        if (cmdp->event != 0xa) {
            return;
        }
        m = (FileShareMsg *)cmdp->u.user_data.data;
        switch (m->mKind) {
        case 1:
            if (m->mFlag == 0) {
                WBT_RequestSync((u16)1, (void *)func_02040f30);
                return;
            }
            child->mReadRegion = m->mFileId;
            child->mRecvBufTable[idx] =
                (int)((FSFile *)data_020a1fc0.mReadRequest)->arg.readfile.dst;
            WBT_GetBlock((u16)1, child->mReadRegion, (const void *)child->mRecvBufTable,
                         ((FSFile *)data_020a1fc0.mReadRequest)->arg.readfile.len,
                         (const void *)child->mPacketBitmapTable, (u32)func_02040f30);
            return;

        case 3: {
            FSFile *p = (FSFile *)data_020a1fc0.mReadRequest;
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
