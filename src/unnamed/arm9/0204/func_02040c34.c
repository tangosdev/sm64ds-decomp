#include "types.h"
#include "private/arm9_a1fc0_file_share.h"

extern int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(int);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(int, int);
extern void _ZN4CP1516DrainWriteBufferEv(void);
extern int func_02061c88(int a, int b, int c);
extern void Crash(void);
extern void func_0205d89c(int x);
extern void FS_InitFile(int *s);
extern void func_020652fc(int a, int b, int c);
extern void func_020424c0(void);
extern void func_0205a588(void *p, int v, int n);
extern int func_020658c0(void *node, int a1, void *a2, int a3, int arg5, int arg6);
extern void func_02042200(void);
extern void func_02041224(char *thiz, int n);
extern void func_02065234(int arg);

extern void func_020417d0(void);
extern void func_020412f0(void);
extern void func_02040f30(void);

extern int data_020a0f84;
extern int data_020a0f88;
extern char data_020a1fc0[];
extern char data_02082158[];

#pragma opt_strength_reduction off
void func_02040c34(int a0, int a1, int a2, int a3, int a4)
{
    FileShareWork *G = (FileShareWork *)data_020a1fc0;
    int state = _ZN3IRQ7DisableEv();
    if (data_020a0f84 != 0) {
        _ZN3IRQ7RestoreEj(state);
    } else {
        data_020a0f84 = 1;
        _ZN3IRQ7RestoreEj(state);

        G->mIsParent = a0;
        G->mPort = (u16)a1;
        G->mSendRequest = 0;
        G->mSendBusy = 0;
        G->mState = 1;
        G->mStateCallback = a2;
        G->mAllocFunc = a3;
        G->mAllocArg = a4;
        G->mReadRequest = 0;
        G->mTableSize = 0;
        G->mTable = 0;
        G->mParentPacketSize = 0;
        G->mChildPacketSize = 0xe;
        data_020a0f88 = 0;
        if (func_02061c88((u16)a1, (int)func_020417d0, 0) != 0)
            Crash();
        func_0205d89c(-1);

        if (G->mIsParent != 0) {
            {
                char *base = (char *)G->mArea;
                char *node;
                char *next;
                char *saved;
                char *zero;
                char *end;
                _ZN4CP1527FlushAndInvalidateDataCacheEjj((int)base, sizeof(FileShareParentArea));
                _ZN4CP1516DrainWriteBufferEv();
                saved = (char *)((FileShareParentArea *)base)->mNodes;
                zero = 0;
                node = saved;
                next = node + FILE_SHARE_NODE_SIZE;
                end = ((int)next & 1) ? (char *)((FileShareParentArea *)base)->mNodes[3]
                                      : (char *)((FileShareParentArea *)base)->mNodes[3];
                ((FileShareParentArea *)base)->mFreeList = (FileShareNode *)saved;
                if (next < end) {
                    do {
                        ((FileShareNode *)node)->mNext =
                            (FileShareNode *)(node + FILE_SHARE_NODE_SIZE);
                        FS_InitFile((int *)((FileShareNode *)node)->mFile);
                        ((FileShareNode *)node)->mRefCount = (int)zero;
                        ((FileShareNode *)node)->mState = (int)zero;
                        ((FileShareNode *)node)->mFileId =
                            (((int)node + 0xcc00cc0) - (int)saved) / 0xcc0;
                        node += FILE_SHARE_NODE_SIZE;
                    } while (node + FILE_SHARE_NODE_SIZE < end);
                }
                ((FileShareNode *)node)->mNext = 0;
                *(int*)(base + 0x2710) = 0;
                ((FileShareParentArea *)base)->mReplyPending = 0;
                ((FileShareParentArea *)base)->mReplySending = 0;
                ((FileShareParentArea *)base)->mReadyList = 0;
                ((FileShareParentArea *)base)->mBusyList = 0;
                G->mParentPacketSize = 0x200;
                ((FileShareParentArea *)base)->mConnectedBitmap = 1;
                ((FileShareParentArea *)base)->mOpenChildren = 0;
                ((FileShareParentArea *)base)->mResizePending = 0;
                ((FileShareParentArea *)base)->mResizeChildren = 0;
                ((FileShareParentArea *)base)->mBusyCount = 0;
                ((FileShareParentArea *)base)->mNewPacketSize = G->mParentPacketSize;
                _ZN4CP1527FlushAndInvalidateDataCacheEjj((int)base, sizeof(FileShareParentArea));
                _ZN4CP1516DrainWriteBufferEv();
                func_020652fc(G->mParentPacketSize, G->mChildPacketSize, (int)func_020412f0);
                func_020424c0();
            }
            {
                char *base = (char *)G->mArea;
                FileShareNode *nd;
                nd = ((FileShareParentArea *)base)->mFreeList;
                ((FileShareParentArea *)base)->mFreeList = nd->mNext;
                func_0205a588(nd, 0, FILE_SHARE_NODE_SIZE);
                /* mTableSize is read through data_020a1fc0, not G, here and in the
                   child branch: the G-> spelling comes out 8 bytes short */
                func_020658c0(nd->mRegion, 0x20000, data_02082158, G->mTable,
                              ((FileShareWork *)data_020a1fc0)->mTableSize, 0);
                nd->mNext = ((FileShareParentArea *)base)->mReadyList;
                ((FileShareParentArea *)base)->mReadyList = nd;
                nd->mRefCount = 1;
                func_02042200();
            }
        } else {
            char *base = (char *)G->mArea;
            FileShareChildArea *arr = (FileShareChildArea *)base;
            int i;
            char *p;
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((int)base, sizeof(FileShareChildArea));
            _ZN4CP1516DrainWriteBufferEv();
            p = base + 0xc0;
            for (i = 0; i < 0x10; i++) {
                arr->mBlockInfoTable[i] = (int)p;
                arr->mRecvBufTable[i] = 0;
                arr->mPacketBitmapTable[i] = 0;
                p += 0x28;
            }
            arr->mRecvWork = 0;
            arr->mReadRegion = 0;
            G->mParentPacketSize = 0x80;
            arr->mMaxReadSize = 0x100000;
            func_02041224(base, -1);
            G->mParentPacketSize = 0;
            func_020424c0();
            func_02041224(base, ((FileShareWork *)data_020a1fc0)->mTableSize);
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((int)base, sizeof(FileShareChildArea));
            _ZN4CP1516DrainWriteBufferEv();
            func_02065234((int)func_02040f30);
        }
    }
    _ZN3IRQ7RestoreEj(state);
}
