#ifndef ARM9_A1FC0_FILE_SHARE_H
#define ARM9_A1FC0_FILE_SHARE_H

#include "private/fs_views.h"

/* Views of the wireless file-sharing work block at data_020a1fc0 (the 0x0204
 * cluster, WFS_End .. WFSi_RomArchiveProc), moved here from func_02040c34.c
 * once WFSi_OnChildSystemCallback, WFSi_OnParentSystemCallback and WFSi_ReallocBitmap needed them too. The
 * transfer runs over NitroSDK WBT (wireless block transfer): the child's
 * tables below are the ones WBT_GetBlockInfo/WBT_GetBlock take, and each
 * parent node embeds the WBTBlockInfoList (nitro/wbt.h) its region is
 * registered with. data_020a2400 is the same address as mArea
 * (data_020a1fc0 + 0x440), a symbol of its own. */

#include "nitro/wm/wm.h"
#include "nitro/wbt.h"

/* Parent area at +0x440: a pool of three 0xcc0-byte nodes, each holding one
 * registered region. A node moves free -> busy (state 1, loading; WFSi_FromFreeToBusy)
 * -> ready (state 2; WFSi_FromBusyToAlive) -> busy (state 3, releasing; WFSi_FromAliveToBusy)
 * -> free (state 0; WFSi_FromBusyToFree). */
enum { FILE_SHARE_NODE_SIZE = 0xcc0 }; /* node slot stride, sizeof(FileShareNode) */

typedef struct FileShareNode {
    struct FileShareNode *mNext; /* 0x00 -- list link (WFSi_MoveList moves nodes) */
    /* 0x04 -- the block list node WBT_RegisterBlock registers the region
     * with (block id, size, 0x20-byte name, data); WFSi_FindAliveForID finds a
     * node by its id */
    WBTBlockInfoList mRegion;
    /* 0x38 -- FS_InitFile here; WFSi_TaskThread opens, reads and closes the
     * region's file through it, WFSi_OnParentSystemCallback refills a page through it.
     * WFSi_FindAlive matches a request against its open extent and
     * WFSi_SendOpenAck sizes the registration by it */
    FSFile mFile;
    int mState; /* 0x7c -- 0 free, 1 loading, 2 ready, 3 releasing */
    /* 0x80 -- WFSi_OnParentSystemCallback: +1 per child reading the region, -1 when it is
     * done; at 0 the node is released */
    int mRefCount;
    /* 0x84 -- WFSi_OnParentSystemCallback: children asking for a region that is still
     * loading, by AID bit */
    int mWaitingChildren;
    /* 0x88 -- WFSi_TaskThread opens the file by it; WFSi_OnParentSystemCallback hands it to
     * the requesting child. Seeded per node in func_02040c34 */
    int mFileId;
    /* 0x8c / 0x90 -- the region's extent in the ROM archive: a child's request
     * (offset, length) is stored here when no ready node holds it
     * (WFSi_FindBusy, which also matches loading nodes by the pair), and
     * WFSi_TaskThread opens [mFileOffset, mFileOffset + mFileSize) with
     * FS_OpenFileDirect. WFSi_FindAlive matches a ready node by the same pair
     * through mFile's top and bottom - top */
    int mFileOffset;
    int mFileSize;
    /* 0x94 -- the block sequence number the previous WBT prepare-send asked
     * for: WFSi_OnParentSystemCallback serves that one when its bytes are cached, then
     * stores the new request's number. WFSi_TaskThread zeroes it on load */
    int mPrevSeqNo;
    /* 0x98 -- page being refilled by an asynchronous read, 3 for none
     * (WFSi_TaskThread loads with 3; WFSi_OnParentSystemCallback sets it to the page it
     * refills and back to 3 once mFile is no longer busy); the cache search
     * skips it */
    int mLoadingPage;
    /* 0x9c -- the last page refilled (WFSi_TaskThread loads with 2):
     * WFSi_OnParentSystemCallback picks the next one round-robin from it */
    int mLastRefillPage;
    /* 0xa0 -- region file offset each page holds: 0, 0x400, 0x800 on load
     * (WFSi_TaskThread), a 0x200-aligned offset on refill (WFSi_OnParentSystemCallback) */
    int mPageOffset[3];
    unsigned char pad_0ac[0x14];
    unsigned char mPages[3][0x400]; /* 0xc0 -- the cached file pages */
} FileShareNode;

/* One file-share message: 9 bytes on the air (WBT_PutUserData), 0xc as a slot
 * of FileShareParentArea::mReplies. mKind is 0 for a child's open request,
 * 1 for its reply, 2 for a child's release request and 3 for its reply. The
 * parent packet size rides split across mPacketSizeHigh (bits 8-11) and
 * mPacketSizeLow. In kinds 1-3 mFileId is the region's registered id
 * (FileShareNode::mFileId, the child's mReadRegion), and mFlag is 1, except
 * on the packet-size notice WFSi_SendAck sends as kind 1 with mFlag 0. The
 * open request (kind 0) carries the region's length in those 24 bits and its
 * offset in the second word instead; WFSi_ReadRequest builds it under those
 * names. */
typedef struct FileShareMsg {
    unsigned int mKind : 4;
    unsigned int mPacketSizeHigh : 4;
    unsigned int mFlag : 24;
    unsigned int mFileId;         /* 0x04 */
    unsigned char mPacketSizeLow; /* 0x08 */
} FileShareMsg;

typedef struct FileShareParentArea {
    /* 0x0000 -- one reply per AID, each a FileShareMsg (above):
     * WFSi_OnParentSystemCallback fills it, WFSi_SendOpenAck sets its file id, WFSi_SendAck
     * sends it */
    unsigned char mReplies[0x10][0xc];
    FileShareNode mNodes[3];                       /* 0x00c0 */
    FileShareNode *mFreeList;                      /* 0x2700 */
    FileShareNode *mReadyList;                     /* 0x2704 -- WFSi_FindAliveForID searches it */
    FileShareNode *mBusyList; /* 0x2708 -- WFSi_TaskThread loads its state-1 nodes */
    /* 0x270c -- children owed a reply, by AID bit (WFSi_OnParentSystemCallback,
     * WFSi_SendOpenAck set; WFSi_SendAck sends and clears) */
    int mReplyPending;
    /* 0x2710 -- children answered as one group, by AID bit: WFSi_SendAck
     * sends to exactly this set once all of it is in mReplyPending, leaves
     * it out of any other send, and clears it when a member's reply no
     * longer matches; func_02040c34 zeroes it */
    int mReplyGroup;
    /* 0x2714 -- WFSi_SendAck sets it after a send and waits; the send
     * callback WFSi_OnSendMessageDone clears it */
    int mReplySending;
    /* 0x2718 -- by AID; bit 0 is the parent itself. WFSi_PortCallback sets a
     * child's bit on connect and clears it on disconnect */
    int mConnectedBitmap;
    /* 0x271c -- children with a region open, by AID bit: WFSi_OnParentSystemCallback sets
     * the bit on each open request (message kind 0); WFSi_SendAck clears it
     * when it answers that child's release (kind 2) and masks it by
     * mConnectedBitmap; func_02040c34 zeroes it */
    int mOpenChildren;
    /* 0x2720 -- WFSi_OnParentSystemCallback: a request named another packet size;
     * WFSi_SendAck applies mNewPacketSize once mBusyCount is 0 */
    int mResizePending;
    /* 0x2724 -- nodes off the free list (WFSi_FromFreeToBusy +1, WFSi_FromBusyToFree -1) */
    int mBusyCount;
    /* 0x2728 -- children to tell the new size, by AID bit (WFSi_OnParentSystemCallback
     * sets, WFSi_SendAck sends) */
    int mResizeChildren;
    int mNewPacketSize; /* 0x272c -- WFSi_SendAck copies it to mParentPacketSize */
} FileShareParentArea;

/* Child area at +0x440. */
typedef struct FileShareChildArea {
    /* 0x000 -- a WBTBlockInfoTable: func_02040c34 points entry i at the 0x28-byte
     * WBTBlockInfo at 0xc0 + i * 0x28; WFSi_OnChildSystemCallback passes the table to
     * WBT_GetBlockInfo (WBT_GetBlockInfo) */
    void *mBlockInfoTable[0x10];
    /* 0x040 -- a WBTRecvBufTable: WFSi_OnChildSystemCallback sets the peer's entry to the
     * receive buffer before WBT_GetBlock (WBT_GetBlock) */
    void *mRecvBufTable[0x10];
    /* 0x080 -- a WBTPacketBitmapTable, WBT_GetBlock's last table: WFSi_ReallocBitmap
     * points entry 0 at mRecvWork */
    void *mPacketBitmapTable[0x10];
    /* 0x0c0 -- the sixteen 0x28-byte WBTBlockInfo mBlockInfoTable points at
     * (func_02040c34) */
    unsigned char mBlockInfo[0x10][0x28];
    /* 0x340 -- WFS_Start's WM_ReadStatus copy of the WM status block;
     * it reads the aid */
    WMStatus mStatus;
    /* 0xc70 -- WFSi_ReallocBitmap allocates it (about four bytes per packet of
     * mMaxReadSize); WFS_End frees it */
    void *mRecvWork;
    int mMaxReadSize; /* 0xc74 -- WFSi_ReallocBitmap grows it to the largest read */
    /* 0xc78 -- WFSi_OnChildSystemCallback: offset of the region being read, as the parent
     * registered it (the tables are 0x20000) */
    int mReadRegion;
} FileShareChildArea;

/* The wireless file-sharing work block at data_020a1fc0. func_02040c34 stores
 * its five arguments in the fields whose comments name them. */
typedef struct FileShareWork {
    int mIsParent;               /* 0x00 -- isParent; the parent builds the node pool at +0x440,
                              the child the receive tables; WFSi_SetMPData picks the
                              parent or child send from it */
    int mSendRequest;            /* 0x04 -- set by WFS_Start/WFSi_OnSetMPDataDone before
                              WFSi_SetMPData, which sends only while it is set */
    int mSendBusy;               /* 0x08 -- WFSi_SetMPData: the port send returned 2 */
    int mState;                  /* 0x0c -- 1 here, 2 once ready (WFS_Start,
                              WFSi_OnChildSystemCallback), 0 at shutdown (WFS_End) */
    void (*mStateCallback)(int); /* 0x10 -- stateCallback; called with 0 when mState becomes 2 */
    void *(*mAllocFunc)(void *arg, unsigned int size,
                        void *ptr); /* 0x14 -- allocFunc; (arg, size, 0) allocates, (arg, 0, ptr)
                              frees (WFSi_ReallocBitmap, WFS_End) */
    void *mAllocArg;                /* 0x18 -- allocArg; first argument to mAllocFunc */
    struct FSFile *mReadRequest;    /* 0x1c -- child read in flight, an FSFile: WFSi_ReadRequest
                              sets it, WFSi_OnChildSystemCallback completes and clears it */
    void *mTable;                   /* 0x20 -- WFSi_LoadTables: the card header's FNT, FAT and
                              overlay tables, read into one allocation */
    unsigned int mTableSize;        /* 0x24 -- that allocation's size */
    int mParentPacketSize; /* 0x28 -- parent MP send size; WFSi_OnChildSystemCallback sets it
                              from the WBT sync's peer packet size + 0xe */
    int mChildPacketSize;  /* 0x2c -- child MP send size, likewise */
    unsigned short mPort;  /* 0x30 -- port; WM_SetPortCallback registers the port
                              callback on it, WFSi_SetMPData sends on it */
    unsigned char pad_032[0xe];
    /* 0x40 -- the MP send buffer, 32-byte aligned (0x020a2000): WFSi_SetMPData
     * builds the next packet in it (WBT's parent or child send hook) and hands
     * it to WM_SetMPDataToPort. Its extent is the gap up to mArea. */
    unsigned char mSendBuf[0x400];
    unsigned char mArea[sizeof(FileShareParentArea)]; /* 0x440 -- FileShareParentArea
                                                         or FileShareChildArea, by
                                                         mIsParent */
} FileShareWork;

typedef char
    FileShareNode_size_must_be_0xcc0[sizeof(FileShareNode) == FILE_SHARE_NODE_SIZE ? 1 : -1];
typedef char
    FileShareParentArea_size_must_be_0x2730[sizeof(FileShareParentArea) == 0x2730 ? 1 : -1];
typedef char FileShareChildArea_size_must_be_0xc7c[sizeof(FileShareChildArea) == 0xc7c ? 1 : -1];
typedef char FileShareMsg_size_must_be_0xc[sizeof(FileShareMsg) == 0xc ? 1 : -1];
typedef char FileShareWork_size_must_be_0x2b70[sizeof(FileShareWork) == 0x2b70 ? 1 : -1];

/* The work block itself, in .bss: it ends at 0x020a4b30, below the next
 * symbol (0x020a4b40). data_020a2400 is its mArea under a symbol of its own. */
extern FileShareWork data_020a1fc0;

/* The block through a pointer cast. Read as data_020a1fc0.mArea, mArea's
 * address folds into a literal of its own (data_020a1fc0+0x440);
 * WFSi_PortCallback and WFSi_SendAck add 0x440 to the block's address instead,
 * which this spelling keeps. */
#define FILE_SHARE_WORK ((FileShareWork *)&data_020a1fc0)

#endif
