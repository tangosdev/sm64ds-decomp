#include "types.h"

/* File-local mirror of include/private/arm9_a9db8_mb_parent.h (the multiboot
 * parent work data_020a9db8 points at): same names and offsets, with the parts
 * this function does not touch left as padding. */
typedef struct MbChildRequest {
    u32 mGameId;    /* 0x00 */
    u8 mUser[0x16]; /* 0x04 */
    u16 mVersion;   /* 0x1a */
    u8 mFileNo;     /* 0x1c */
    u8 pad_1d[0x3];
} MbChildRequest;

typedef struct MbParentFile {
    u8 mHeader[0x59c];    /* 0x000 */
    u8 mBlockTable[0x14]; /* 0x59c */
    u8 *mGameRegistry;    /* 0x5b0 */
    s32 mSrcAddr;         /* 0x5b4 */
    u16 mCurrentBlock;    /* 0x5b8 */
    u16 mRequestedBlock;  /* 0x5ba */
    u16 mChildBitmap;     /* 0x5bc */
    u16 mEntryBitmap;     /* 0x5be */
    u16 mEntryChanged;    /* 0x5c0 */
    u8 mActive;           /* 0x5c2 */
    u8 pad_5c3[0x1];
} MbParentFile;

typedef struct MbParentWork {
    u8 pad_0000[0x1340];
    u8 mChildUser[15][0x16]; /* 0x1340 */
    u16 mChildVersion[15];   /* 0x148a */
    u32 mChildGameId[15];    /* 0x14a8 */
    u8 pad_14e4[0x4];
    s32 mChildState[15];              /* 0x14e8 */
    u8 mFileCount;                    /* 0x1524 */
    u8 mCurrentFile;                  /* 0x1525 */
    s8 mChildFile[15];                /* 0x1526 */
    u8 mChildCount;                   /* 0x1535 */
    u16 mChildEntryBitmap;            /* 0x1536 */
    MbChildRequest mChildRequest[15]; /* 0x1538 */
    u32 mChildRequestPieces[15];      /* 0x1718 */
    u16 mChildResponse[15];           /* 0x1754 */
    u8 pad_1772[0x1788 - 0x1772];
    MbParentFile mFiles[16]; /* 0x1788 */
} MbParentWork;

/* NitroSDK MBGameRegistry, the game record a file was registered with
 * (MbParentFile.mGameRegistry; func_02067928 stores its first argument there
 * and hands the same pointer to MBi_MakeGameInfo, func_02068680). Same shape
 * and names as func_02068680's view. */
typedef struct MBGameRegistry {
    int romFilePathp;           /* 0x00 */
    int gameNamep;              /* 0x04 -- u16 string */
    int gameIntroductionp;      /* 0x08 -- u16 string */
    int iconCharPathp;          /* 0x0c */
    int iconPalettePathp;       /* 0x10 */
    int ggid;                   /* 0x14 -- matched against the child's mChildGameId */
    unsigned char maxPlayerNum; /* 0x18 -- entries (parent included) at or past it are refused */
} MBGameRegistry;

/* NitroSDK WMMpRecvData, one child's slot of an MP receive buffer: what
 * func_020664b4 gets back from func_020619ac (WM_ReadMPData) for each AID and
 * passes here once its length is neither 0 nor 0xffff. */
typedef struct WMMpRecvData {
    u16 length;     /* 0x00 */
    u16 rate;       /* 0x02 */
    u16 rssi;       /* 0x04 */
    u16 aid;        /* 0x06 */
    u16 noResponse; /* 0x08 */
    u16 cdata[1];   /* 0x0a -- the child's packet */
} WMMpRecvData;

/* NitroSDK MBCommChildBlockHeader, a child packet unpacked by func_02065c2c
 * (MBi_SetRecvBufferFromChild): the type byte, then either the block number a
 * child asks for (types 8 and 9) or one piece of its entry request (type 7,
 * which func_02065de4 files into mChildRequest). */
typedef struct MBCommChildBlockHeader {
    u8 type; /* 0x00 */
    u8 pad1[1];
    union {
        struct {
            u16 req; /* 0x02 */
        } data;
        struct {
            u8 piece;      /* 0x02 */
            u8 data[0x10]; /* 0x03 */
            u8 pad2[1];
        } req_data;
    } u;
} MBCommChildBlockHeader;

#ifndef SM64DS_PLATFORM_PC
/* The SDK packs it to 0x14: 2 + the 0x12-byte request piece. */
typedef char
    MBCommChildBlockHeader_size_must_be_0x14[sizeof(MBCommChildBlockHeader) == 0x14 ? 1 : -1];
#endif

extern u8 *data_020a9db8;

/* Typed through a cast: most declarations of data_020a9db8 spell it as a byte
 * pointer, and a typed extern here disagrees with them. */
#define MB_WORK ((MbParentWork *)data_020a9db8)

extern void func_02065c2c(u8 *a, u8 *b);
extern int func_02065de4(void *a, u32 b);
extern void CpuCopy8(void *src, void *dst, int n);
extern int func_02066fec(u32 a, int b, int c);
extern u16 func_02065ef8(u16 a, u16 b);

/* File record i, reached the way the ROM builds its address: the work pointer
 * stepped a whole MbParentFile per index, then mFiles[0] from there. Writing
 * MB_WORK->mFiles[i] builds it differently and moves bytes, except for
 * mRequestedBlock in message 9. rec below is the same stepped pointer. */
#define MB_FILE(i) (((MbParentWork *)(data_020a9db8 + (i) * sizeof(MbParentFile)))->mFiles)

/* NitroSDK MBi_CommParentRecvDataPerChild(arg, child): one child's packet
 * from an MP receive. hd is the unpacked block header; req is the local copy
 * of the child's assembled entry request (MBCommRequestData), filed into the
 * per-child tables once func_02065de4 reports every piece in. */
void func_0206655c(void *arg0, u32 arg1)
{
    MBCommChildBlockHeader hd;
    MbChildRequest req;
    int phase;
    int idx;
    u8 cnt;
    u8 physIdx;
    MbParentWork *rec;
    MBGameRegistry *p;
    u8 bidx;
    u16 flags;
    u32 bit;
    u8 pi;
    u16 *fld;
    int gv;
    u8 *cp;
    u32 i;

    func_02065c2c((u8 *)((WMMpRecvData *)arg0)->cdata, (u8 *)&hd);
    if (arg1 == 0)
        return;
    if (arg1 > 0xf)
        return;

    idx = arg1 - 1;
    phase = MB_WORK->mChildState[idx];

    switch (hd.type) {
    case 7:
        if (phase == 2 && func_02065de4(&hd, arg1) == 1) {
            i = arg1 - 1;
            CpuCopy8(&MB_WORK->mChildRequest[i], &req, 0x1d);
            MB_WORK->mChildGameId[i] = req.mGameId;
            MB_WORK->mChildVersion[i] = req.mVersion;
            CpuCopy8(req.mUser, MB_WORK->mChildUser[i], 0x16);
            {
                u8 *pp = MB_WORK->mChildUser[i];
                *pp = (u8)((*pp & ~0xf0) | (((u8)arg1 & 0xf) << 4));
            }
            func_02066fec(arg1, 0xa, (int)req.mUser);
        }
        if (phase != 0xa)
            return;
        cnt = 0;
        physIdx = MB_WORK->mChildRequest[idx].mFileNo;
        /* have/disp gotos kept: an inline session predicate grows it (0x554); only an
           || chain assigning rec/p inside the condition matches */
        if (physIdx < 0x10) {
            rec = (MbParentWork *)(data_020a9db8 + physIdx * sizeof(MbParentFile));
            if (rec->mFiles[0].mActive != 0) {
                p = (MBGameRegistry *)rec->mFiles[0].mGameRegistry;
                gv = MB_WORK->mChildGameId[idx];
                if (gv == p->ggid)
                    goto have;
            }
        }
        MB_WORK->mChildResponse[idx] = 4;
        goto disp;
    have:
        flags = rec->mFiles[0].mEntryBitmap;
        for (bidx = 0; (u32)bidx < 0x10; bidx++) {
            if ((1 << bidx) & flags)
                cnt++;
        }
        if ((u32)cnt >= (u32)p->maxPlayerNum) {
            MB_WORK->mChildResponse[idx] = 0;
            func_02066fec(arg1, 0xb, 0);
            return;
        }
    disp:
        switch (MB_WORK->mChildResponse[idx]) {
        case 3:
            bit = 1u << arg1;
            if (MB_WORK->mChildEntryBitmap & bit)
                return;
            cp = &MB_WORK->mChildCount;
            (*cp)++;
            fld = &MB_WORK->mChildEntryBitmap;
            *fld = (u16)(*fld | bit);
            MB_WORK->mChildFile[arg1 - 1] = physIdx;
            fld = &MB_FILE(physIdx)->mEntryBitmap;
            *fld = (u16)(*fld | bit);
            fld = &MB_FILE(physIdx)->mEntryChanged;
            *fld = (u16)(*fld | bit);
            MB_WORK->mChildResponse[arg1 - 1] = 0;
            func_02066fec(arg1, 5, 0);
            return;
        case 4:
            MB_WORK->mChildResponse[idx] = 0;
            func_02066fec(arg1, 4, 0);
            return;
        }
        return;

    case 8:
        if (phase == 5) {
            func_02066fec(arg1, 0xe, 0);
            return;
        }
        if (phase != 0xe)
            return;
        if (MB_WORK->mChildResponse[idx] != 2)
            return;
        pi = MB_WORK->mChildFile[idx];
        fld = &MB_FILE(pi)->mChildBitmap;
        *fld = (u16)(*fld | (1u << arg1));
        MB_FILE(pi)->mCurrentBlock = 0;
        MB_WORK->mChildResponse[idx] = 0;
        func_02066fec(arg1, 6, 0);
        return;

    case 9:
        if (phase != 6)
            return;
        pi = MB_WORK->mChildFile[idx];
        if (pi == 0xff)
            return;
        MB_WORK->mFiles[pi].mRequestedBlock =
            func_02065ef8(MB_WORK->mFiles[pi].mRequestedBlock, hd.u.data.req);
        return;

    case 10:
        if (phase == 6) {
            pi = MB_WORK->mChildFile[idx];
            if (pi == 0xff)
                return;
            fld = &MB_FILE(pi)->mChildBitmap;
            *fld = (u16)(*fld & ~(1u << arg1));
            func_02066fec(arg1, 7, 0);
            return;
        }
        if (phase != 7)
            return;
        if (MB_WORK->mChildResponse[idx] != 5)
            return;
        MB_WORK->mChildResponse[idx] = 0;
        func_02066fec(arg1, 8, 0);
        return;

    case 11:
        break;
    }
}
