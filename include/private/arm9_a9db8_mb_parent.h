#ifndef ARM9_A9DB8_MB_PARENT_H
#define ARM9_A9DB8_MB_PARENT_H
#include "types.h"

/* The wireless-download parent work block that data_020a9db8 points at
 * (arm9 func_02065c2c .. MBi_ParentCallback). Only the fields those functions
 * read are placed; everything else is padding at its measured offset.
 *
 * Message types 3/4 sent and 7/8/9 received, fifteen per-child state words
 * at 0x14e8 and the sixteen-entry file table below are the NitroSDK
 * multiboot (MB) parent layout, so the names follow that role without
 * claiming the SDK's own identifiers. */

/* NitroSDK MbSegmentInfo: one row of a download file's segment table
 * (ARM9 static, ARM7 static, ROM header), as MBi_SetSegmentInfo
 * (MBi_SetSegmentInfo) fills it. */
typedef struct MbSegmentInfo {
    u32 recv_addr;  /* 0x00 -- where the child receives it: load_addr, or a
                       staging buffer for an ARM7 segment */
    u32 load_addr;  /* 0x04 -- where it finally runs */
    u32 size;       /* 0x08 */
    u32 target : 1; /* 0x0c -- set for the ARM7 segment */
    u32 rsv : 31;
} MbSegmentInfo;

/* NitroSDK MbDownloadFileInfoHeader, the start of MbParentFile.mHeader
 * (MBi_MakeDownloadFileInfo builds it). */
typedef struct MbDownloadFileInfoHeader {
    u32 arm9EntryAddr;    /* 0x00 */
    u32 arm7EntryAddr;    /* 0x04 */
    u32 padding;          /* 0x08 */
    MbSegmentInfo seg[3]; /* 0x0c */
} MbDownloadFileInfoHeader;

/* NitroSDK MB_BlockInfoTable, MbParentFile.mBlockTable: each segment's byte
 * offset and first block number in the stream, and the total block count. */
typedef struct MB_BlockInfoTable {
    u32 seg_head_offset[3]; /* 0x00 */
    u16 block_head[3];      /* 0x0c */
    u16 block_num;          /* 0x12 */
} MB_BlockInfoTable;

typedef struct MbParentFile {
    u8 mHeader[0x59c];    /* 0x000 -- download info; MBi_CommParentSendDlFileInfo sends 0xe4 of
                             it, func_02067530 reads three 16-byte segment rows
                             at +0xc */
    u8 mBlockTable[0x14]; /* 0x59c -- NitroSDK MB_BlockInfoTable: 3 segment
                             byte offsets (running sums of the segment sizes),
                             3 head block numbers, block count; built by
                             MBi_MakeBlockInfoTable, read by func_02067530 */
    u8 *mGameRegistry;    /* 0x5b0 -- the NitroSDK MBGameRegistry the file was
                             registered with: MB_RegisterFile stores its first
                             argument here (and refuses it twice) and builds
                             the MBGameInfo from it at +0xe4 (MBi_MakeGameInfo);
                             MBi_CommParentRecvDataPerChild checks a child's ggid (+0x14) and
                             maxPlayerNum (+0x18) in it */
    s32 mSrcAddr;         /* 0x5b4 -- added to a block's offset before MI_CpuCopy8 */
    u16 mCurrentBlock;    /* 0x5b8 -- block number being sent */
    u16 mRequestedBlock;  /* 0x5ba -- highest block a child asked for this
                             cycle: MBi_CommParentRecvData zeroes it, MBi_CommParentRecvDataPerChild
                             raises it per request, MBi_Calc_SendBlock keeps
                             mCurrentBlock within two of it */
    u16 mChildBitmap;     /* 0x5bc -- children waiting on this file */
    u16 mEntryBitmap;     /* 0x5be -- bit 0 (the parent) at registration, plus
                             each child accepted into the file (MBi_CommParentRecvDataPerChild);
                             its population is checked against the player limit */
    u16 mEntryChanged;    /* 0x5c0 -- children accepted or dropped since the
                             last update: MBi_CommParentCallback passes it and
                             mEntryBitmap to MB_UpdateGameInfoMember, then clears it */
    u8  mActive;          /* 0x5c2 -- nonzero once the file is registered */
    u8  pad_5c3[0x1];
} MbParentFile;

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char MbParentFile_size_must_be_0x5c4[sizeof(MbParentFile) == 0x5c4 ? 1 : -1];
#endif

/* The entry request a child sends. MBi_ReceiveRequestDataPiece assembles it piece by piece
 * into mChildRequest; once complete, MBi_CommParentRecvDataPerChild copies its 0x1d bytes out
 * and files the parts into the per-child tables below. */
typedef struct MbChildRequest {
    u32 mGameId;    /* 0x00 -- copied to mChildGameId */
    u8 mUser[0x16]; /* 0x04 -- copied to mChildUser */
    u16 mVersion;   /* 0x1a -- copied to mChildVersion */
    u8 mFileNo;     /* 0x1c -- the mFiles index it asks to enter */
    u8 pad_1d[0x3];
} MbChildRequest;

#ifndef SM64DS_PLATFORM_PC
/* mChildRequest[15] runs 0x1538..0x1718, so one request is 0x20. */
typedef char MbChildRequest_size_must_be_0x20[sizeof(MbChildRequest) == 0x20 ? 1 : -1];
#endif

typedef struct MbParentWork {
    /* 0x0000 -- the MP buffers: MB_Init points the WM work's sendBuf at
     * mSendBuf and its recvBuf at mRecvBuf */
    u8 mSendBuf[0x400];
    u8 mRecvBuf[0xf00];
    /* 0x1300 -- the parent's own MBUserInfo (NitroSDK parentInfo):
     * MB_RegisterFile hands it to MBi_MakeGameInfo, which copies its 0x16 bytes
     * into each registered file's game record */
    u8 mParentUser[0x16];
    /* 0x1316 -- the multiboot work is set up (the BW2 reconstruction's
     * mbc->isMbInitialized): MB_Init refuses while it is set and sets it last,
     * and the WM_End result clears it on either side (MBi_EndCommon) */
    u16 mIsMbInitialized;
    /* 0x1318 -- bytes of a file carried per block: MB_StartParent sets it to
     * the MP parent size less 6; MBi_MakeBlockInfoTable divides each segment by it
     * and func_02067530 turns a block number back into an offset with it */
    u32 mBlockSize;
    /* 0x131c -- bytes of the child's entry request carried per piece:
     * MB_StartParent sets it to the MP child size less 2, and
     * MBi_ReceiveRequestDataPiece copies each received piece at piece * mReqPieceSize */
    s32 mReqPieceSize;
    /* 0x1320 -- pieces a whole request takes, 30 / mReqPieceSize
     * (MB_StartParent): MBi_ReceiveRequestDataPiece refuses a piece number above it and
     * counts the received bits up to it */
    s32 mReqPieceCount;
    /* 0x1324 -- a WM_StartMP is in flight (the BW2 reconstruction's
     * mbc->start_mp_busy): MBi_ParentCallback sets it before starting MP
     * on a child's CONNECTED, the START_MP result clears it */
    s32 mStartMpBusy;
    u8 pad_1328[0x1340 - 0x1328];
    /* 0x1340 -- per child (AID - 1): the user record from its entry request,
     * AID in the high nibble of byte 0 (MBi_CommParentRecvDataPerChild);
     * MBi_CommParentCallback hands the table to MB_UpdateGameInfoMember */
    u8 mChildUser[15][0x16];
    /* 0x148a -- the entry request's u16 at +0x1a; cleared on disconnect, not
     * read back in this cluster (medium) */
    u16 mChildVersion[15];
    /* 0x14a8 -- the game id the child asked for; matched against its file's
     * mGameRegistry +0x14 */
    u32 mChildGameId[15];
    /* 0x14e4 -- NitroSDK parent_callback: MB_CommSetParentStateCallback
     * stores it, MBi_CommChangeParentStateCallbackOnly calls it with (child, state, arg) when it is
     * set, and MB_StartParent saves it across its clear of the work */
    u32 mParentCallback;
    /* 0x14e8 -- per-child state: MBi_CommChangeParentState sets it, MB_CommGetParentState
     * returns it, and MB_CommIsBootable tests it for 7 (send complete) */
    s32 mChildState[15];
    /* 0x1524 -- registered files: MB_RegisterFile refuses a 17th and counts
     * each one; zero means nothing to send */
    u8 mFileCount;
    u8 mCurrentFile;   /* 0x1525 -- mFiles index, stepped mod 16 per send */
    s8 mChildFile[15]; /* 0x1526 -- mFiles index each child entered, -1 none */
    /* 0x1535 -- children accepted (MBi_CommParentRecvDataPerChild +1, MBi_CommParentCallback -1) */
    u8 mChildCount;
    u16 mChildEntryBitmap;            /* 0x1536 -- the same children, by AID bit */
    MbChildRequest mChildRequest[15]; /* 0x1538 -- each child's entry request */
    /* 0x1718 -- pieces of it received, by bit (MBi_ReceiveRequestDataPiece); cleared on
     * disconnect */
    u32 mChildRequestPieces[15];
    /* 0x1754 -- the application's answer to an entry request
     * (MB_CommResponseRequest); MBi_CommParentRecvDataPerChild acts on 3 (accept)
     * and 4 (refuse) for message 7, 2 for message 8 and 5 for message 10, and
     * clears it */
    u16 mChildResponse[15];
    /* 0x1772 -- MB_CommGetChildUser's copy of the user record it returns */
    u8 mChildUserBuf[0x16];
    MbParentFile mFiles[16]; /* 0x1788 */
} MbParentWork;

#endif
