#ifndef ARM9_FILE_SHARE_VIEWS_H
#define ARM9_FILE_SHARE_VIEWS_H

#include "private/arm9_a1fc0_file_share.h"

/* Pointer table. FileShareChildArea spells these slots int. */
struct ChildTables {
    char *mBlockInfoTable[0x10];
    int mRecvBufTable[0x10];
    int mPacketBitmapTable[0x10];
    unsigned char pad_0c0[0xbb0];
    int mRecvWork;
    int mMaxReadSize;
    int mReadRegion;
};

/* mNodes is FileShareNode[3]. The shared header stores the bytes. */
struct ParentNodes {
    unsigned char mReplies[0x10][0xc];
    FileShareNode mNodes[3];
};

#endif
