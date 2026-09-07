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
    u8  mBlockTable[0x14];/* 0x59c -- 3 segment addresses, 3 head block numbers,
                             block count (func_02067530) */
    u8  pad_5b0[0x4];
    s32 mSrcAddr;         /* 0x5b4 -- added to a block's offset before CpuCopy8 */
    u16 mCurrentBlock;    /* 0x5b8 -- block number being sent */
    u16 unk_5ba;          /* 0x5ba -- func_02065f08 keeps mCurrentBlock within
                             two of it */
    u16 mChildBitmap;     /* 0x5bc -- children waiting on this file */
    u8  pad_5be[0x4];
    u8  mActive;          /* 0x5c2 -- nonzero once the file is registered */
    u8  pad_5c3[0x1];
} MbParentFile;

typedef char MbParentFile_size_must_be_0x5c4[sizeof(MbParentFile) == 0x5c4 ? 1 : -1];

typedef struct MbParentWork {
    u8  pad_0000[0x1524];
    u8  unk_1524;         /* 0x1524 -- zero means nothing to send */
    u8  mCurrentFile;     /* 0x1525 -- mFiles index, stepped mod 16 per send */
    u8  pad_1526[0x1788 - 0x1526];
    MbParentFile mFiles[16]; /* 0x1788 */
} MbParentWork;

#endif
