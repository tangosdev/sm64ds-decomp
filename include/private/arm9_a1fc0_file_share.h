#ifndef ARM9_A1FC0_FILE_SHARE_H
#define ARM9_A1FC0_FILE_SHARE_H

/* Views of the wireless file-sharing work block at data_020a1fc0 (the 0x0204
 * cluster, func_02040aa4 .. func_02042668), moved here from func_02040c34.c
 * once func_02040f30, func_020412f0 and func_02041224 needed them too. The
 * transfer runs over NitroSDK WBT (wireless block transfer): the child's
 * tables below are the ones WBT_GetBlockInfo/WBT_GetBlock take. Plain C types:
 * func_020412f0 and func_02041224 do not include types.h. data_020a2400 is
 * the same address as mArea (data_020a1fc0 + 0x440), a symbol of its own. */

/* Parent area at +0x440: a pool of three 0xcc0-byte nodes, each holding one
 * registered region. A node moves free -> busy (state 1, loading; func_02041b60)
 * -> ready (state 2; func_02041b24) -> busy (state 3, releasing; func_02041ae8)
 * -> free (state 0; func_02041a94). */
enum { FILE_SHARE_NODE_SIZE = 0xcc0 }; /* node slot stride, sizeof(FileShareNode) */

typedef struct FileShareNode {
    struct FileShareNode *mNext; /* 0x00 -- list link (func_02041bbc moves nodes) */
    /* 0x04 -- func_020658c0 registers it (offset, size, 0x20-byte name,
     * data); func_02041c2c finds a node by its offset */
    unsigned char mRegion[0x34];
    /* 0x38 -- FS_InitFile here; func_02042254 opens, reads and closes the
     * region's file through it, func_020412f0 refills a page through it */
    unsigned char mFile[0x44];
    int mState; /* 0x7c -- 0 free, 1 loading, 2 ready, 3 releasing */
    /* 0x80 -- func_020412f0: +1 per child reading the region, -1 when it is
     * done; at 0 the node is released */
    int mRefCount;
    /* 0x84 -- func_020412f0: children asking for a region that is still
     * loading, by AID bit */
    int mWaitingChildren;
    /* 0x88 -- func_02042254 opens the file by it; func_020412f0 hands it to
     * the requesting child. Seeded per node in func_02040c34 */
    int mFileId;
    /* 0x8c / 0x90 -- the region's extent in the ROM archive: a child's request
     * (offset, length) is stored here when no ready node holds it
     * (func_02041c64, which also matches loading nodes by the pair), and
     * func_02042254 opens [mFileOffset, mFileOffset + mFileSize) with
     * FS_OpenFileDirect. func_02041ce0 matches a ready node by the same pair
     * through mFile's top and bottom - top */
    int mFileOffset;
    int mFileSize;
    /* 0x94 -- the block sequence number the previous WBT prepare-send asked
     * for: func_020412f0 serves that one when its bytes are cached, then
     * stores the new request's number. func_02042254 zeroes it on load */
    int mPrevSeqNo;
    /* 0x98 -- page being refilled by an asynchronous read, 3 for none
     * (func_02042254 loads with 3; func_020412f0 sets it to the page it
     * refills and back to 3 once mFile is no longer busy); the cache search
     * skips it */
    int mLoadingPage;
    /* 0x9c -- the last page refilled (func_02042254 loads with 2):
     * func_020412f0 picks the next one round-robin from it */
    int mLastRefillPage;
    /* 0xa0 -- region file offset each page holds: 0, 0x400, 0x800 on load
     * (func_02042254), a 0x200-aligned offset on refill (func_020412f0) */
    int mPageOffset[3];
    unsigned char pad_0ac[0x14];
    unsigned char mPages[3][0x400]; /* 0xc0 -- the cached file pages */
} FileShareNode;

typedef struct FileShareParentArea {
    /* 0x0000 -- one reply per AID: func_020412f0 fills it, func_02041d28 sets
     * its file id, func_02041e14 sends it */
    unsigned char mReplies[0x10][0xc];
    unsigned char mNodes[3][FILE_SHARE_NODE_SIZE]; /* 0x00c0 */
    FileShareNode *mFreeList;                      /* 0x2700 */
    FileShareNode *mReadyList;                     /* 0x2704 -- func_02041c2c searches it */
    FileShareNode *mBusyList; /* 0x2708 -- func_02042254 loads its state-1 nodes */
    /* 0x270c -- children owed a reply, by AID bit (func_020412f0,
     * func_02041d28 set; func_02041e14 sends and clears) */
    int mReplyPending;
    /* 0x2710 -- children answered as one group, by AID bit: func_02041e14
     * sends to exactly this set once all of it is in mReplyPending, leaves
     * it out of any other send, and clears it when a member's reply no
     * longer matches; func_02040c34 zeroes it */
    int mReplyGroup;
    /* 0x2714 -- func_02041e14 sets it after a send and waits; the send
     * callback func_02042160 clears it */
    int mReplySending;
    /* 0x2718 -- by AID; bit 0 is the parent itself. func_020417d0 sets a
     * child's bit on connect and clears it on disconnect */
    int mConnectedBitmap;
    /* 0x271c -- children with a region open, by AID bit: func_020412f0 sets
     * the bit on each open request (message kind 0); func_02041e14 clears it
     * when it answers that child's release (kind 2) and masks it by
     * mConnectedBitmap; func_02040c34 zeroes it */
    int mOpenChildren;
    /* 0x2720 -- func_020412f0: a request named another packet size;
     * func_02041e14 applies mNewPacketSize once mBusyCount is 0 */
    int mResizePending;
    /* 0x2724 -- nodes off the free list (func_02041b60 +1, func_02041a94 -1) */
    int mBusyCount;
    /* 0x2728 -- children to tell the new size, by AID bit (func_020412f0
     * sets, func_02041e14 sends) */
    int mResizeChildren;
    int mNewPacketSize; /* 0x272c -- func_02041e14 copies it to mParentPacketSize */
} FileShareParentArea;

/* Child area at +0x440. */
typedef struct FileShareChildArea {
    /* 0x000 -- a WBTBlockInfoTable: func_02040c34 points entry i at the 0x28-byte
     * WBTBlockInfo at 0xc0 + i * 0x28; func_02040f30 passes the table to
     * WBT_GetBlockInfo (func_02065788) */
    int mBlockInfoTable[0x10];
    /* 0x040 -- a WBTRecvBufTable: func_02040f30 sets the peer's entry to the
     * receive buffer before WBT_GetBlock (func_02065730) */
    int mRecvBufTable[0x10];
    /* 0x080 -- a WBTPacketBitmapTable, WBT_GetBlock's last table: func_02041224
     * points entry 0 at mRecvWork */
    int mPacketBitmapTable[0x10];
    unsigned char pad_0c0[0xbb0];
    /* 0xc70 -- func_02041224 allocates it (about four bytes per packet of
     * mMaxReadSize); func_02040aa4 frees it */
    int mRecvWork;
    int mMaxReadSize; /* 0xc74 -- func_02041224 grows it to the largest read */
    /* 0xc78 -- func_02040f30: offset of the region being read, as the parent
     * registered it (the tables are 0x20000) */
    int mReadRegion;
} FileShareChildArea;

/* The wireless file-sharing work block at data_020a1fc0. */
typedef struct FileShareWork {
    int mIsParent;         /* 0x00 -- a0; the parent builds the node pool at +0x440,
                              the child the receive tables; func_02041930 picks the
                              parent or child send from it */
    int mSendRequest;      /* 0x04 -- set by func_02040bb0/func_020418f0 before
                              func_02041930, which sends only while it is set */
    int mSendBusy;         /* 0x08 -- func_02041930: the port send returned 2 */
    int mState;            /* 0x0c -- 1 here, 2 once ready (func_02040bb0,
                              func_02040f30), 0 at shutdown (func_02040aa4) */
    int mStateCallback;    /* 0x10 -- a2; called with 0 when mState becomes 2 */
    int mAllocFunc;        /* 0x14 -- a3; (arg, size, 0) allocates, (arg, 0, ptr)
                              frees (func_02041224, func_02040aa4) */
    int mAllocArg;         /* 0x18 -- a4; first argument to mAllocFunc */
    int mReadRequest;      /* 0x1c -- child read in flight, an FSFile: func_02041a00
                              sets it, func_02040f30 completes and clears it */
    int mTable;            /* 0x20 -- func_020424c0: the card header's FNT, FAT and
                              overlay tables, read into one allocation */
    int mTableSize;        /* 0x24 -- that allocation's size */
    int mParentPacketSize; /* 0x28 -- parent MP send size; func_02040f30 sets it
                              from the WBT sync's peer packet size + 0xe */
    int mChildPacketSize;  /* 0x2c -- child MP send size, likewise */
    unsigned short mPort;  /* 0x30 -- a1; func_02061c88 registers the port
                              callback on it, func_02041930 sends on it */
    unsigned char pad_032[0x40e];
    unsigned char mArea[sizeof(FileShareParentArea)]; /* 0x440 -- FileShareParentArea
                                                         or FileShareChildArea, by
                                                         mIsParent */
} FileShareWork;

typedef char
    FileShareNode_size_must_be_0xcc0[sizeof(FileShareNode) == FILE_SHARE_NODE_SIZE ? 1 : -1];
typedef char
    FileShareParentArea_size_must_be_0x2730[sizeof(FileShareParentArea) == 0x2730 ? 1 : -1];
typedef char FileShareChildArea_size_must_be_0xc7c[sizeof(FileShareChildArea) == 0xc7c ? 1 : -1];
typedef char FileShareWork_size_must_be_0x2b70[sizeof(FileShareWork) == 0x2b70 ? 1 : -1];

/* The work block through its view; each consumer declares data_020a1fc0. */
#define FILE_SHARE_WORK ((FileShareWork *)data_020a1fc0)

#endif
