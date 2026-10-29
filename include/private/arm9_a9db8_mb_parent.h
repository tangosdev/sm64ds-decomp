#ifndef ARM9_A9DB8_MB_PARENT_H
#define ARM9_A9DB8_MB_PARENT_H
#include "types.h"

/* The wireless-download parent work block that data_020a9db8 points at
 * (arm9 func_02065c2c .. func_02069994). Only the fields those functions
 * read are placed; everything else is padding at its measured offset.
 *
 * Message types 3/4 sent and 7/8/9 received, fifteen per-child state words
 * at 0x14e8 and the sixteen-entry file table below are the NitroSDK
 * multiboot (MB) parent layout, so the names follow that role without
 * claiming the SDK's own identifiers. */

typedef struct MbParentFile {
    u8  mHeader[0x59c];   /* 0x000 -- download info; func_020662c0 sends 0xe4 of
                             it, func_02067530 reads three 16-byte segment rows
                             at +0xc */
    u8 mBlockTable[0x14]; /* 0x59c -- NitroSDK MB_BlockInfoTable: 3 segment
                             byte offsets (running sums of the segment sizes),
                             3 head block numbers, block count; built by
                             func_02067604, read by func_02067530 */
    u8 *mGameRegistry;    /* 0x5b0 -- the NitroSDK MBGameRegistry the file was
                             registered with: func_02067928 stores its first
                             argument here (and refuses it twice) and builds
                             the MBGameInfo from it at +0xe4 (func_02068680);
                             func_0206655c checks a child's ggid (+0x14) and
                             maxPlayerNum (+0x18) in it */
    s32 mSrcAddr;         /* 0x5b4 -- added to a block's offset before CpuCopy8 */
    u16 mCurrentBlock;    /* 0x5b8 -- block number being sent */
    u16 mRequestedBlock;  /* 0x5ba -- highest block a child asked for this
                             cycle: func_020664b4 zeroes it, func_0206655c
                             raises it per request, func_02065f08 keeps
                             mCurrentBlock within two of it */
    u16 mChildBitmap;     /* 0x5bc -- children waiting on this file */
    u16 mEntryBitmap;     /* 0x5be -- bit 0 (the parent) at registration, plus
                             each child accepted into the file (func_0206655c);
                             its population is checked against the player limit */
    u16 mEntryChanged;    /* 0x5c0 -- children accepted or dropped since the
                             last update: func_02066a94 passes it and
                             mEntryBitmap to func_0206853c, then clears it */
    u8  mActive;          /* 0x5c2 -- nonzero once the file is registered */
    u8  pad_5c3[0x1];
} MbParentFile;

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char MbParentFile_size_must_be_0x5c4[sizeof(MbParentFile) == 0x5c4 ? 1 : -1];
#endif

/* The entry request a child sends. func_02065de4 assembles it piece by piece
 * into mChildRequest; once complete, func_0206655c copies its 0x1d bytes out
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
    u8 pad_0000[0x1300];
    /* 0x1300 -- the parent's own MBUserInfo (NitroSDK parentInfo):
     * func_02067928 hands it to func_02068680, which copies its 0x16 bytes
     * into each registered file's game record */
    u8 mParentUser[0x16];
    u8 pad_1316[0x2];
    /* 0x1318 -- bytes of a file carried per block: func_02068ae8 sets it to
     * the MP parent size less 6; func_02067604 divides each segment by it
     * and func_02067530 turns a block number back into an offset with it */
    u32 mBlockSize;
    /* 0x131c -- bytes of the child's entry request carried per piece:
     * func_02068ae8 sets it to the MP child size less 2, and
     * func_02065de4 copies each received piece at piece * mReqPieceSize */
    s32 mReqPieceSize;
    /* 0x1320 -- pieces a whole request takes, 30 / mReqPieceSize
     * (func_02068ae8): func_02065de4 refuses a piece number above it and
     * counts the received bits up to it */
    s32 mReqPieceCount;
    u8 pad_1324[0x1340 - 0x1324];
    /* 0x1340 -- per child (AID - 1): the user record from its entry request,
     * AID in the high nibble of byte 0 (func_0206655c); func_02066a94 hands
     * the table to func_0206853c */
    u8 mChildUser[15][0x16];
    /* 0x148a -- the entry request's u16 at +0x1a; cleared on disconnect, not
     * read back in this cluster (medium) */
    u16 mChildVersion[15];
    /* 0x14a8 -- the game id the child asked for; matched against its file's
     * mGameRegistry +0x14 */
    u32 mChildGameId[15];
    /* 0x14e4 -- NitroSDK parent_callback: func_020672a4 stores it,
     * func_02066fb0 calls it with (child, state, arg) when it is set, and
     * func_02068ae8 saves it across its clear of the work */
    u32 mParentCallback;
    /* 0x14e8 -- per-child state: func_02066fec sets it, func_02067250
     * returns it */
    s32 mChildState[15];
    /* 0x1524 -- registered files: func_02067928 refuses a 17th and counts
     * each one; zero means nothing to send */
    u8 mFileCount;
    u8 mCurrentFile;   /* 0x1525 -- mFiles index, stepped mod 16 per send */
    s8 mChildFile[15]; /* 0x1526 -- mFiles index each child entered, -1 none */
    /* 0x1535 -- children accepted (func_0206655c +1, func_02066a94 -1) */
    u8 mChildCount;
    u16 mChildEntryBitmap;            /* 0x1536 -- the same children, by AID bit */
    MbChildRequest mChildRequest[15]; /* 0x1538 -- each child's entry request */
    /* 0x1718 -- pieces of it received, by bit (func_02065de4); cleared on
     * disconnect */
    u32 mChildRequestPieces[15];
    /* 0x1754 -- the application's answer to an entry request
     * (func_0206703c); func_0206655c acts on 3 (accept) and 4 (refuse) for
     * message 7, 2 for message 8 and 5 for message 10, and clears it */
    u16 mChildResponse[15];
    u8 pad_1772[0x1788 - 0x1772];
    MbParentFile mFiles[16]; /* 0x1788 */
} MbParentWork;

#endif
