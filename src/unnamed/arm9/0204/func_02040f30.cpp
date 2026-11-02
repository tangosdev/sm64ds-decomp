//cpp
#include "types.h"
#include "private/arm9_a1fc0_file_share.h"
#include "private/arm9_wbt.h"
#include "CP15.h"
#include "private/fs_views.h"

/* The FSFile a child read is for (FileShareWork.mReadRequest, set by
 * func_02041a00). */
typedef struct FSFile FSFile;

extern "C" {

extern int func_02065a4c(int x);
extern void func_02065788(u16 type, u32 arg1, const void *src, u32 arg3);
extern void func_02041a00(FSFile *file);
extern void func_02065730(u16 type, u32 arg1, const void *src1, u32 arg3, const void *src2,
                          u32 extra);
extern int func_020423dc(int *table);
extern int func_020656d4(u16 arg0, const void *arg1, int arg2, u32 arg3);
extern void func_020657d0(u16 id, void *obj);
extern void FS_NotifyArchiveAsyncEnd(void *self, int arg);
extern void func_02042160(void);
extern void func_02040f30(void *arg);
extern char data_020a1fc0[];
extern char data_020a2400[];
void func_02040f30(void *arg) {
    WBTCommand *cmdp = (WBTCommand *)arg;
    FileShareChildArea *child;
    int idx;
    int type;
    idx = func_02065a4c(cmdp->peer_bmp);
    type = cmdp->command;
    child = (FileShareChildArea *)data_020a2400;
    switch (type) {
    case 2:
        if (cmdp->target_bmp != 0) {
            return;
        }
        FILE_SHARE_WORK->mParentPacketSize = cmdp->u.sync.peer_packet_size + 0xe;
        FILE_SHARE_WORK->mChildPacketSize = cmdp->u.sync.my_packet_size + 0xe;
        if (FILE_SHARE_WORK->mState != 2) {
            func_02065788((u16)1, 0, (const void *)child->mBlockInfoTable, (u32)func_02040f30);
            return;
        }
        {
            int request = FILE_SHARE_WORK->mReadRequest;
            if (request != 0) {
                func_02041a00((FSFile *)request);
            }
        }
        return;

    case 6:
        child->mRecvBufTable[idx] = FILE_SHARE_WORK->mTable;
        func_02065730((u16)1, 0x20000, (const void *)child->mRecvBufTable,
                      (u32)FILE_SHARE_WORK->mTableSize, (const void *)child->mPacketBitmapTable,
                      (u32)func_02040f30);
        return;

    case 4:
        if (cmdp->target_bmp != 0) {
            return;
        }
        if ((FILE_SHARE_WORK->mState != 2) != 0) {
            CP15::FlushAndInvalidateDataCache((u32)FILE_SHARE_WORK->mTable,
                                              (u32)FILE_SHARE_WORK->mTableSize);
            func_020423dc((int *)FILE_SHARE_WORK->mTable);
            {
                FILE_SHARE_WORK->mState = 2;
                {
                    int fn = FILE_SHARE_WORK->mStateCallback;
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
            c = FILE_SHARE_WORK->mParentPacketSize;
            cmd.mFileId = w4;
            cmd.mPacketSizeHigh = (u8)(c >> 8);
            cmd.mPacketSizeLow = (u8)c;
            func_020656d4((u16)1, &cmd, 9, (u32)func_02042160);
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
                func_020657d0((u16)1, (void *)func_02040f30);
                return;
            }
            child->mReadRegion = m->mFileId;
            child->mRecvBufTable[idx] =
                (int)((FSFile *)FILE_SHARE_WORK->mReadRequest)->arg.readfile.dst;
            func_02065730((u16)1, child->mReadRegion, (const void *)child->mRecvBufTable,
                          ((FSFile *)FILE_SHARE_WORK->mReadRequest)->arg.readfile.len,
                          (const void *)child->mPacketBitmapTable, (u32)func_02040f30);
            return;

        case 3: {
            FSFile *p = (FSFile *)FILE_SHARE_WORK->mReadRequest;
            void *arc = p->arc;
            FILE_SHARE_WORK->mReadRequest = 0;
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
