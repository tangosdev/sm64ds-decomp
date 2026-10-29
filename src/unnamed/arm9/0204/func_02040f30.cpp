//cpp
#include "types.h"
#include "private/arm9_a1fc0_file_share.h"
#include "private/arm9_wbt.h"
#include "CP15.h"

extern "C" {

typedef struct {
    u32 kind : 4;
    u32 chan : 4;
    u32 flag : 24;
    u32 word4;
    u8 byte8;
} Cmd;

/* The FSFile a child read is for (FileShareWork.mReadRequest, set by
 * func_02041a00). This ROM's FSFile has its prop union at 0x1c and its arg
 * union at 0x2c (see func_0205c264.c); names follow the SDK's FSFile. */
typedef struct FSFile {
    char pad_00[8];
    void *arc; /* 0x08 -- FS_NotifyArchiveAsyncEnd (func_0205cd5c) takes it */
    char pad_0c[0x1c];
    u32 pos; /* 0x28 -- prop.file.pos */
    u32 dst; /* 0x2c -- arg.readfile.dst */
    char pad_30[4];
    u32 len; /* 0x34 -- arg.readfile.len */
} FSFile;

extern int func_02065a4c(int x);
extern void func_02065788(u16 type, u32 arg1, const void *src, u32 arg3);
extern void func_02041a00(int x);
extern void func_02065730(u16 type, u32 arg1, const void *src1, u32 arg3, const void *src2,
                          u32 extra);
extern int func_020423dc(int *r6);
extern int func_020656d4(u16 arg0, const void *arg1, int arg2, u32 arg3);
extern void func_020657d0(u16 id, void *obj);
extern void func_0205cd5c(void *self, int arg);
extern void func_02042160(void);
extern void func_02040f30(void *arg0);
extern char data_020a1fc0[];
extern char data_020a2400[];
void func_02040f30(void *arg0) {
    WBTCommand *cmdp = (WBTCommand *)arg0;
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
                func_02041a00(request);
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
            Cmd cmd;
            cmd.kind = 2;
            cmd.flag = 1;
            c = FILE_SHARE_WORK->mParentPacketSize;
            cmd.word4 = w4;
            cmd.chan = (u8)(c >> 8);
            cmd.byte8 = (u8)c;
            func_020656d4((u16)1, &cmd, 9, (u32)func_02042160);
        }
        return;

    case 0xc: {
        Cmd *m;
        if (cmdp->event != 0xa) {
            return;
        }
        m = (Cmd *)cmdp->u.user_data.data;
        switch (m->kind) {
        case 1:
            if (m->flag == 0) {
                func_020657d0((u16)1, (void *)func_02040f30);
                return;
            }
            child->mReadRegion = m->word4;
            child->mRecvBufTable[idx] = ((FSFile *)FILE_SHARE_WORK->mReadRequest)->dst;
            func_02065730((u16)1, child->mReadRegion, (const void *)child->mRecvBufTable,
                          ((FSFile *)FILE_SHARE_WORK->mReadRequest)->len,
                          (const void *)child->mPacketBitmapTable, (u32)func_02040f30);
            return;

        case 3: {
            FSFile *p = (FSFile *)FILE_SHARE_WORK->mReadRequest;
            void *a0 = p->arc;
            FILE_SHARE_WORK->mReadRequest = 0;
            p->pos += p->len;
            func_0205cd5c(a0, 0);
            return;
        }
        }

        return;
    }
    }
}
}
