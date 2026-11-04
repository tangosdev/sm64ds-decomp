//cpp
/* The wireless-download parent's protocol (the Nitro SDK's mb_parent.c),
 * arm9 .text 0x02065eb8..0x020672d0.
 *
 * pPwork (data_020a9db8, the MbParentWork of
 * private/arm9_a9db8_mb_parent.h) holds a state word per child (AID 1..15)
 * and a sixteen-entry file table. MBi_CommParentCallback is the WM parent
 * callback: it moves children through their states on connect and
 * disconnect, hands each MP receive to MBi_CommParentRecvData, and on each
 * send turn MBi_CommParentSendData picks what goes out: a state message to
 * the children waiting on one, a file's download info, or the next block of
 * a file. The application sees each state change through the callback it
 * stores with MB_CommSetParentStateCallback, and answers entry requests with
 * MB_CommResponseRequest.
 *
 * Names: pret's pmd-sky keeps this file as assembly in overlay_00, and
 * every function here except MB_CommGetChildrenNumber is matched to one of
 * its functions by body. MB_CommGetChildrenNumber is not in pmd-sky. It takes
 * the SDK's public mb.h name for what its body returns, the count of
 * accepted children. mwccarm emits the bodies in reverse, so
 * MB_CommSetParentStateCallback, the first in source, is the highest in the
 * ROM.
 *
 * This SDK is older than pmd-sky's. It has no any_recv_bitmap:
 * MBi_Calc_SendBlock steps every active file with children waiting, not only
 * those a child asked a block of in this receive. It has no block cache
 * either: MBi_CommParentSendBlock copies straight from the file's source
 * address, and pmd-sky's MBi_ReloadCache is absent.
 */
#include "types.h"
#include "IRQ.h"
#include "private/arm9_a9db8_mb_parent.h"
#include "private/mb_comm_child_block_header.h"

extern "C" {

/* The SDK's MB_COMM_PSTATE_* values this file uses. */
#define MB_COMM_PSTATE_INIT_COMPLETE 1
#define MB_COMM_PSTATE_CONNECTED 2
#define MB_COMM_PSTATE_DISCONNECTED 3
#define MB_COMM_PSTATE_KICKED 4
#define MB_COMM_PSTATE_REQ_ACCEPTED 5
#define MB_COMM_PSTATE_SEND_PROCEED 6
#define MB_COMM_PSTATE_SEND_COMPLETE 7
#define MB_COMM_PSTATE_BOOT_REQUEST 8
#define MB_COMM_PSTATE_BOOT_STARTABLE 9
#define MB_COMM_PSTATE_REQUESTED 10
#define MB_COMM_PSTATE_MEMBER_FULL 11
#define MB_COMM_PSTATE_END 12
#define MB_COMM_PSTATE_ERROR 13
#define MB_COMM_PSTATE_WAIT_TO_SEND 14

/* The SDK's pPwork; char * is the spelling the other MB parent files share. */
extern char *data_020a9db8;
/* The file the last download info went out for (MBi_CommParentSendDlFileInfo). */
extern u8 data_0209a078;

/* Typed through a cast: most declarations of data_020a9db8 spell it as a byte
 * pointer, and a typed extern here disagrees with them. */
#define MB_WORK ((MbParentWork *)data_020a9db8)

/* File record i, reached the way the ROM builds its address: the work pointer
 * stepped a whole MbParentFile per index, then mFiles[0] from there. Writing
 * MB_WORK->mFiles[i] builds it differently and moves bytes, except for
 * mRequestedBlock in message 9. */
#define MB_FILE(i) (((MbParentWork *)(data_020a9db8 + (i) * sizeof(MbParentFile)))->mFiles)

/* Child i's response, reached the same stepped way, one halfword per index.
 * Compiled as C++, a plain MB_WORK->mChildResponse[idx] pools 0x1754 and
 * indexes by register instead of the cartridge's +0x1700, #0x54.
 * MBi_CommParentRecvDataPerChild needs two respellings to match: this macro
 * for its switch read (plain, one word longer) and [aid - 1] for case 10's
 * write (through [idx], two words longer). */
#define MB_RESPONSE(i) (((MbParentWork *)(data_020a9db8 + (i) * sizeof(u16)))->mChildResponse[0])

typedef void (*MBCommPStateCallback)(u32 child_aid, u32 status, void *arg);

/* The argument of an MB_COMM_PSTATE_ERROR report. */
typedef struct MBErrorStatus {
    u16 errcode;
} MBErrorStatus;

/* NitroSDK MBCommParentBlockHeader, the header func_02065d5c
 * (MBi_MakeParentSendBuffer) packs in front of a parent packet: the type
 * byte, then for a block (type 4) its file and block number. */
typedef struct MBCommParentBlockHeader {
    u8 type;
    u8 pad1[1];
    u16 fid;
    u16 seqno;
} MBCommParentBlockHeader;

/* NitroSDK MB_BlockInfo, one block's place as func_02067530
 * (pmd-sky's MBi_Get_BlockInfo) gives it. The tag is that file's. */
struct Out {
    int child_recv_buff_addr; /* 0x0 */
    u32 size;                 /* 0x4 */
    int offset;               /* 0x8 */
    u8 segment_no;            /* 0xc */
};

/* NitroSDK MBGameRegistry, the game record a file was registered with
 * (MbParentFile.mGameRegistry). */
typedef struct MBGameRegistry {
    int romFilePathp;           /* 0x00 */
    int gameNamep;              /* 0x04 -- u16 string */
    int gameIntroductionp;      /* 0x08 -- u16 string */
    int iconCharPathp;          /* 0x0c */
    int iconPalettePathp;       /* 0x10 */
    int ggid;                   /* 0x14 -- matched against the child's mChildGameId */
    unsigned char maxPlayerNum; /* 0x18 -- entries (parent included) at or past it are refused */
} MBGameRegistry;

/* NitroSDK WMMpRecvData, one child's slot of an MP receive buffer, as
 * WM_ReadMPData returns it. */
typedef struct WMMpRecvData {
    u16 length;     /* 0x00 */
    u16 rate;       /* 0x02 */
    u16 rssi;       /* 0x04 */
    u16 aid;        /* 0x06 */
    u16 noResponse; /* 0x08 */
    u16 cdata[1];   /* 0x0a -- the child's packet */
} WMMpRecvData;

/* Leading fields of NitroSDK WMStartParentCallback, the argument the WM
 * parent callback (func_02069994) passes for CONNECTED (code 0),
 * DISCONNECTED (code 1) and BEACON_SENT (code 0x1c). Codes 0xff (a failed
 * WM call, from MBi_CheckWmErrcode: {apiid, errcode}) and 0x100 (any other WM
 * result) read only the WMCallback header the SDK callbacks all share. */
typedef struct WmStartParentCallback {
    u16 apiid;        /* 0x00 */
    u16 errcode;      /* 0x02 */
    u16 wlCmdID;      /* 0x04 */
    u16 wlResult;     /* 0x06 */
    u16 state;        /* 0x08 */
    u8 macAddress[6]; /* 0x0a */
    u16 aid;          /* 0x10 */
} WmStartParentCallback;

extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern void MultiStore32Bytes(unsigned val, int *dst, int len);
extern void CpuCopy8(const void *src, void *dst, u32 size);
extern void func_0205a588(void *dst, u8 val, u32 size);
extern char *WM_ReadMPData(char *buf, unsigned int aid);
extern u8 *func_02065c2c(u8 *src, u8 *dst);
extern u8 *func_02065d5c(MBCommParentBlockHeader *hdr, u8 *buf);
extern int func_02065de4(u8 *hdr, int aid);
extern int func_02067530(struct Out *bi, char *table, u32 block, char *dlinfo);
/* local extern: defined void, but it hands back func_020688fc's result,
   which the senders here return. */
extern int func_02067f2c(u32 size, u32 pollbmp, u32 buf);
extern void MB_SendGameInfoBeacon(void);
extern void MB_UpdateGameInfoMember(char *self, void *src, u32 flags, int s);
extern int func_0206881c(void);

void MBi_CommCallParentError(u16 aid, u16 errcode);
int IsChildAidValid(u32 aid);
u32 MBi_Calc_NextSendBlock(u32 now, u32 req);
void MBi_Calc_SendBlock(int fid);
void MBi_CommParentSendData(void);
int MBi_CommParentSendBlock(void);
int MBi_CommParentSendDlFileInfo(void);
int MBi_CommParentSendMsg(u8 type, u32 pollbmp);
void MBi_CommParentRecvData(char *arg);
void MBi_CommParentRecvDataPerChild(void *arg, u32 aid);
void MBi_CommChangeParentStateCallbackOnly(u32 aid, int state, void *arg);
void MBi_CommChangeParentState(u32 aid, int state, void *arg);

// @symbol MB_CommSetParentStateCallback
void MB_CommSetParentStateCallback(u32 callback) {
    u32 saved = IRQ::Disable();
    MB_WORK->mParentCallback = callback;
    IRQ::Restore(saved);
}

// @symbol MB_CommGetParentState
int MB_CommGetParentState(u16 aid) {
    if (data_020a9db8 != 0 && IsChildAidValid(aid))
        return MB_WORK->mChildState[aid - 1];
    return 0;
}

// @symbol MB_CommGetChildUser
void *MB_CommGetChildUser(u16 aid) {
    u32 saved = IRQ::Disable();
    if (data_020a9db8 != 0 && IsChildAidValid(aid)) {
        CpuCopy8(MB_WORK->mChildUser[aid - 1], MB_WORK->mChildUserBuf, 0x16);
        IRQ::Restore(saved);
        return MB_WORK->mChildUserBuf;
    }
    IRQ::Restore(saved);
    return 0;
}

// @symbol MB_CommGetChildrenNumber
u8 MB_CommGetChildrenNumber(void) {
    if (data_020a9db8)
        return MB_WORK->mChildCount;
    return 0;
}

/* The child at aid can be booted once it has the whole download. */
// @symbol MB_CommIsBootable
int MB_CommIsBootable(u16 aid) {
    if (data_020a9db8 != 0 && IsChildAidValid(aid) &&
        MB_WORK->mChildState[aid - 1] == MB_COMM_PSTATE_SEND_COMPLETE)
        return 1;
    return 0;
}

/* The application's answer to a child: 0 kick, 1 accept (both for an entry
 * request), 2 start sending, 3 boot. Stored for MBi_CommParentRecvDataPerChild
 * only while the child is in the state that answer is for. */
// @symbol MB_CommResponseRequest
int MB_CommResponseRequest(u16 aid, int ack) {
    int response;
    int state;
    u32 saved = IRQ::Disable();
    switch (ack) {
    case 0:
        state = MB_COMM_PSTATE_REQUESTED;
        response = 4;
        break;
    case 1:
        state = MB_COMM_PSTATE_REQUESTED;
        response = 3;
        break;
    case 2:
        state = MB_COMM_PSTATE_WAIT_TO_SEND;
        response = 2;
        break;
    case 3:
        state = MB_COMM_PSTATE_SEND_COMPLETE;
        response = 5;
        break;
    default:
        IRQ::Restore(saved);
        return 0;
    }
    if (data_020a9db8 != 0 && IsChildAidValid(aid)) {
        u32 i = aid - 1;
        if (MB_WORK->mChildState[i] == state) {
            MB_WORK->mChildResponse[i] = (u16)response;
            IRQ::Restore(saved);
            return 1;
        }
    }
    IRQ::Restore(saved);
    return 0;
}

// @symbol MBi_CommChangeParentState
void MBi_CommChangeParentState(u32 aid, int state, void *arg) {
    if (IsChildAidValid(aid))
        MB_WORK->mChildState[aid - 1] = state;
    MBi_CommChangeParentStateCallbackOnly(aid, state, arg);
}

// @symbol MBi_CommChangeParentStateCallbackOnly
void MBi_CommChangeParentStateCallbackOnly(u32 aid, int state, void *arg) {
    MBCommPStateCallback cb = (MBCommPStateCallback)MB_WORK->mParentCallback;
    if (cb)
        cb(aid, state, arg);
}

/* The WM parent callback. */
// @symbol MBi_CommParentCallback
void MBi_CommParentCallback(int type, char *arg) {
    WmStartParentCallback *cb = (WmStartParentCallback *)arg;

    switch (type) {
    case 0x15:
        MBi_CommChangeParentState(0, MB_COMM_PSTATE_INIT_COMPLETE, arg);
        break;
    case 0x11:
        MBi_CommChangeParentState(0, MB_COMM_PSTATE_END, 0);
        break;
    case 0: {
        u16 v = cb->aid;
        if (v != 0 && v < 0x10) {
            MBi_CommChangeParentState(v, MB_COMM_PSTATE_CONNECTED, arg);
        }
        break;
    }
    case 1: {
        u16 v = cb->aid;
        if (v != 0 && v < 0x10) {
            MB_WORK->mChildVersion[v - 1] = 0;
            func_0205a588(&MB_WORK->mChildGameId[cb->aid - 1], 0, 4);
            func_0205a588(MB_WORK->mChildUser[cb->aid - 1], 0, 0x16);
            {
                int cur = func_0206881c();
                func_0205a588(&MB_WORK->mChildRequest[cb->aid - 1], 0,
                              (cur - 2) * 0x1e / (func_0206881c() - 2));
            }
            MB_WORK->mChildRequestPieces[cb->aid - 1] = 0;
            {
                u16 id = cb->aid;
                s8 slot = MB_WORK->mChildFile[id - 1];
                if (slot != -1) {
                    u32 off = (u8)slot * sizeof(MbParentFile);
                    u32 mask = ~(1 << id);
                    ((MbParentWork *)(data_020a9db8 + off))->mFiles->mEntryBitmap &= mask;
                    ((MbParentWork *)(data_020a9db8 + off))->mFiles->mEntryChanged |= (1 << id);
                    MB_WORK->mChildFile[id - 1] = -1;
                    ((MbParentWork *)(data_020a9db8 + off))->mFiles->mChildBitmap &= mask;
                }
            }
            if (MB_WORK->mChildEntryBitmap & (1 << cb->aid)) {
                MB_WORK->mChildCount -= 1;
                MB_WORK->mChildEntryBitmap &= ~(1 << cb->aid);
            }
            {
                u16 id2 = cb->aid;
                if (MB_WORK->mChildState[id2 - 1] == MB_COMM_PSTATE_BOOT_REQUEST) {
                    MBi_CommChangeParentState(id2, MB_COMM_PSTATE_BOOT_STARTABLE, 0);
                }
            }
            MBi_CommChangeParentState(cb->aid, MB_COMM_PSTATE_DISCONNECTED, arg);
            MB_WORK->mChildState[cb->aid - 1] = 0;
        }
        break;
    }
    case 3:
        MBi_CommParentRecvData(arg);
        break;
    case 0x19:
        MBi_CommParentSendData();
        break;
    case 0x1c: {
        u8 i;
        for (i = 0; i < 0x10; i++) {
            MbParentWork *e = (MbParentWork *)(data_020a9db8 + i * (int)sizeof(MbParentFile));
            if (e->mFiles->mActive != 0) {
                u16 f48 = e->mFiles->mEntryChanged;
                if (f48 != 0) {
                    /* the game info record MB_UpdateGameInfoMember refreshes sits after
                     * the 0xe4 bytes of mHeader that MBi_CommParentSendDlFileInfo
                     * sends */
                    MB_UpdateGameInfoMember((char *)&MB_WORK->mFiles[i].mHeader[0xe4],
                                            MB_WORK->mChildUser, e->mFiles->mEntryBitmap, f48);
                    /* the signed-index stepped form; MB_FILE (unsigned sizeof)
                     * or e reallocates registers */
                    ((MbParentWork *)(data_020a9db8 + i * (int)sizeof(MbParentFile)))
                        ->mFiles->mEntryChanged = 0;
                }
            }
        }
        MB_SendGameInfoBeacon();
        break;
    }
    case 0xff: {
        u16 v = cb->errcode;
        switch (v) {
        case 1:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
            MBi_CommCallParentError(0, 9);
            break;
        default:
        case 0:
        case 2:
        case 3:
        case 7:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            MBi_CommCallParentError(0, 8);
            break;
        }
        break;
    }
    case 0x100: {
        u16 v = cb->apiid;
        switch (v) {
        case 0:
        case 7:
        case 8:
        case 13:
        case 14:
        case 15:
        case 17:
        case 18:
        case 21:
        case 25:
        case 29:
            MBi_CommCallParentError(0, 9);
            break;
        default:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 9:
        case 10:
        case 11:
        case 12:
        case 16:
        case 19:
        case 20:
        case 22:
        case 23:
        case 24:
        case 26:
        case 27:
        case 28:
            MBi_CommCallParentError(0, 8);
            break;
        }
        break;
    }
    }

    if (type != 0x11) {
        return;
    }
    {
        volatile int sp0 = 0;
        MultiStore32Bytes(sp0, (int *)data_020a9db8, 0x73e0);
    }
    data_020a9db8 = 0;
}

/* One child's packet from an MP receive. hd is the unpacked block header;
 * req is the local copy of the child's assembled entry request
 * (MBCommRequestData), filed into the per-child tables once func_02065de4
 * reports every piece in. */
// @symbol MBi_CommParentRecvDataPerChild
void MBi_CommParentRecvDataPerChild(void *arg, u32 aid) {
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

    func_02065c2c((u8 *)((WMMpRecvData *)arg)->cdata, (u8 *)&hd);
    if (aid == 0)
        return;
    if (aid > 0xf)
        return;

    idx = aid - 1;
    phase = MB_WORK->mChildState[idx];

    switch (hd.type) {
    case 7:
        if (phase == MB_COMM_PSTATE_CONNECTED && func_02065de4((u8 *)&hd, aid) == 1) {
            i = aid - 1;
            CpuCopy8(&MB_WORK->mChildRequest[i], &req, 0x1d);
            MB_WORK->mChildGameId[i] = req.mGameId;
            MB_WORK->mChildVersion[i] = req.mVersion;
            CpuCopy8(req.mUser, MB_WORK->mChildUser[i], 0x16);
            {
                u8 *pp = MB_WORK->mChildUser[i];
                *pp = (u8)((*pp & ~0xf0) | (((u8)aid & 0xf) << 4));
            }
            MBi_CommChangeParentState(aid, MB_COMM_PSTATE_REQUESTED, req.mUser);
        }
        if (phase != MB_COMM_PSTATE_REQUESTED)
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
            MBi_CommChangeParentState(aid, MB_COMM_PSTATE_MEMBER_FULL, 0);
            return;
        }
    disp:
        switch (MB_RESPONSE(idx)) {
        case 3:
            bit = 1u << aid;
            if (MB_WORK->mChildEntryBitmap & bit)
                return;
            cp = &MB_WORK->mChildCount;
            (*cp)++;
            fld = &MB_WORK->mChildEntryBitmap;
            *fld = (u16)(*fld | bit);
            MB_WORK->mChildFile[aid - 1] = physIdx;
            fld = &MB_FILE(physIdx)->mEntryBitmap;
            *fld = (u16)(*fld | bit);
            fld = &MB_FILE(physIdx)->mEntryChanged;
            *fld = (u16)(*fld | bit);
            MB_WORK->mChildResponse[aid - 1] = 0;
            MBi_CommChangeParentState(aid, MB_COMM_PSTATE_REQ_ACCEPTED, 0);
            return;
        case 4:
            MB_WORK->mChildResponse[idx] = 0;
            MBi_CommChangeParentState(aid, MB_COMM_PSTATE_KICKED, 0);
            return;
        }
        return;

    case 8:
        if (phase == MB_COMM_PSTATE_REQ_ACCEPTED) {
            MBi_CommChangeParentState(aid, MB_COMM_PSTATE_WAIT_TO_SEND, 0);
            return;
        }
        if (phase != MB_COMM_PSTATE_WAIT_TO_SEND)
            return;
        if (MB_WORK->mChildResponse[idx] != 2)
            return;
        pi = MB_WORK->mChildFile[idx];
        fld = &MB_FILE(pi)->mChildBitmap;
        *fld = (u16)(*fld | (1u << aid));
        MB_FILE(pi)->mCurrentBlock = 0;
        MB_WORK->mChildResponse[idx] = 0;
        MBi_CommChangeParentState(aid, MB_COMM_PSTATE_SEND_PROCEED, 0);
        return;

    case 9:
        if (phase != MB_COMM_PSTATE_SEND_PROCEED)
            return;
        pi = MB_WORK->mChildFile[idx];
        if (pi == 0xff)
            return;
        MB_WORK->mFiles[pi].mRequestedBlock =
            MBi_Calc_NextSendBlock(MB_WORK->mFiles[pi].mRequestedBlock, hd.u.data.req);
        return;

    case 10:
        if (phase == MB_COMM_PSTATE_SEND_PROCEED) {
            pi = MB_WORK->mChildFile[idx];
            if (pi == 0xff)
                return;
            fld = &MB_FILE(pi)->mChildBitmap;
            *fld = (u16)(*fld & ~(1u << aid));
            MBi_CommChangeParentState(aid, MB_COMM_PSTATE_SEND_COMPLETE, 0);
            return;
        }
        if (phase != MB_COMM_PSTATE_SEND_COMPLETE)
            return;
        if (MB_WORK->mChildResponse[idx] != 5)
            return;
        MB_WORK->mChildResponse[aid - 1] = 0; /* [aid - 1], not [idx]: see MB_RESPONSE */
        MBi_CommChangeParentState(aid, MB_COMM_PSTATE_BOOT_REQUEST, 0);
        return;

    case 11:
        break;
    }
}

/* An MP receive: clear each registered file's requested block, then pass
 * every child's nonempty packet on. */
// @symbol MBi_CommParentRecvData
void MBi_CommParentRecvData(char *arg) {
    u16 i;
    for (i = 0; i < 0x10; i++) {
        if (MB_FILE(i)->mActive != 0)
            MB_FILE(i)->mRequestedBlock = 0;
    }
    for (i = 1; i <= 0xf; i++) {
        u16 *p = (u16 *)WM_ReadMPData(arg, i);
        if (p == 0)
            continue;
        if (*p == 0xffff)
            continue;
        if (*p == 0)
            continue;
        MBi_CommParentRecvDataPerChild(p, i);
    }
}

// @symbol MBi_CommParentSendMsg
int MBi_CommParentSendMsg(u8 type, u32 pollbmp) {
    MBCommParentBlockHeader hd;
    hd.type = type;
    func_02065d5c(&hd, (u8 *)data_020a9db8);
    return func_02067f2c(6, pollbmp, (u32)data_020a9db8);
}

/* Send the download info of the next file (mod 16) that a child in
 * REQ_ACCEPTED has entered, to every such child of that file. */
// @symbol MBi_CommParentSendDlFileInfo
int MBi_CommParentSendDlFileInfo(void) {
    MBCommParentBlockHeader hd;
    u8 count[16];
    char found = -1;
    u16 n;
    u16 mask = 0;
    u8 c;
    u8 k;
    u8 *ret;

    func_0205a588(count, mask, 16);

    for (n = 1; n <= 15; n++) {
        if (MB_WORK->mChildState[n - 1] == MB_COMM_PSTATE_REQ_ACCEPTED) {
            s8 t = MB_WORK->mChildFile[n - 1];
            count[t]++;
        }
    }

    c = data_0209a078;
    for (k = 0; k < 16; k++) {
        c = (c + 1) % 16;
        if (MB_WORK->mFiles[c].mActive != 0) {
            if (count[c] != 0) {
                found = c;
                break;
            }
        }
    }

    if (found == -1)
        return 0x15;

    data_0209a078 = found;
    for (n = 1; n <= 15; n++) {
        if (MB_WORK->mChildState[n - 1] == MB_COMM_PSTATE_REQ_ACCEPTED) {
            if (found == MB_WORK->mChildFile[n - 1])
                mask |= 1 << n;
        }
    }

    hd.type = 3;
    hd.fid = found;
    ret = func_02065d5c(&hd, (u8 *)data_020a9db8);
    if (ret != 0)
        CpuCopy8(MB_WORK->mFiles[found].mHeader, ret, 0xe4);

    return func_02067f2c(0xea, mask, (u32)data_020a9db8);
}

/* Typed access through private/arm9_a9db8_mb_parent.h is the match. The raw
 * char* form this replaces pooled the whole 0x1525 address once the byte was
 * read from more than one statement (+12 bytes), and with that fixed by hand
 * a named `g + 0x1000` base still rotated five registers in the three reads
 * before CpuCopy8; a struct field at 0x1525 makes that base a compiler temp
 * and the colouring falls into place. */
// @symbol MBi_CommParentSendBlock
int MBi_CommParentSendBlock(void) {
    unsigned int loopCount;
    u8 idxByte;
    MBCommParentBlockHeader hd;
    struct Out bi;

    if (MB_WORK->mFileCount == 0)
        return 0x15;

    loopCount = 0;
    do {
        {
            /* re-read through a volatile: MB_WORK is one word shorter */
            MbParentWork *g = *(MbParentWork *volatile *)&data_020a9db8;
            g->mCurrentFile = (g->mCurrentFile + 1) % 16;
        }
        idxByte = MB_WORK->mCurrentFile;

        if (MB_WORK->mFiles[idxByte].mActive != 0 && MB_WORK->mFiles[idxByte].mChildBitmap != 0)
            break;

        loopCount = (loopCount + 1) & 0xff;
    } while (loopCount < 0x10);
    if (loopCount == 0x10)
        return 0x15;

    MBi_Calc_SendBlock(idxByte);

    idxByte = MB_WORK->mCurrentFile;
    if (func_02067530(&bi, (char *)MB_WORK->mFiles[idxByte].mBlockTable,
                      MB_WORK->mFiles[idxByte].mCurrentBlock,
                      (char *)MB_WORK->mFiles[idxByte].mHeader) == 0) {
        return 0x15;
    }

    {
        MbParentWork *g = MB_WORK;
        bi.offset = bi.offset + g->mFiles[g->mCurrentFile].mSrcAddr;
        hd.type = 4;
        hd.fid = g->mCurrentFile;
        hd.seqno = g->mFiles[g->mCurrentFile].mCurrentBlock;
    }

    CpuCopy8((void *)bi.offset, func_02065d5c(&hd, (u8 *)data_020a9db8), bi.size);

    idxByte = MB_WORK->mCurrentFile;
    return func_02067f2c(bi.size + 6, MB_WORK->mFiles[idxByte].mChildBitmap, (u32)data_020a9db8);
}

/* A send turn: sort the children by state into AID bitmaps and send one
 * thing, the first of: BOOT_REQUEST's message 5, CONNECTED's message 1,
 * MEMBER_FULL's message 6, KICKED's message 2, REQ_ACCEPTED's download info,
 * or a block. With nothing to send, message 0 goes to everyone. */
// @symbol MBi_CommParentSendData
void MBi_CommParentSendData(void) {
    u16 bmp[6];
    u16 i;
    int result;

    bmp[0] = 0;
    MultiStore16(bmp[0], (char *)&bmp[1], 10);

    for (i = 1; i <= 0xf; i++) {
        switch (MB_WORK->mChildState[i - 1]) {
        case MB_COMM_PSTATE_CONNECTED:
            bmp[1] |= (1 << i);
            break;
        case MB_COMM_PSTATE_REQ_ACCEPTED:
            bmp[2] |= (1 << i);
            break;
        case MB_COMM_PSTATE_KICKED:
            bmp[3] |= (1 << i);
            break;
        case MB_COMM_PSTATE_BOOT_REQUEST:
            bmp[4] |= (1 << i);
            break;
        case MB_COMM_PSTATE_MEMBER_FULL:
            bmp[5] |= (1 << i);
            break;
        }
    }

    if (bmp[4])
        result = MBi_CommParentSendMsg(5, bmp[4]);
    else if (bmp[1])
        result = MBi_CommParentSendMsg(1, bmp[1]);
    else if (bmp[5])
        result = MBi_CommParentSendMsg(6, bmp[5]);
    else if (bmp[3])
        result = MBi_CommParentSendMsg(2, bmp[3]);
    else if (bmp[2])
        result = MBi_CommParentSendDlFileInfo();
    else
        result = MBi_CommParentSendBlock();

    if (result == 0x15)
        MBi_CommParentSendMsg(0, 0xffff);
}

/* Step file fid's block cursor: while mCurrentBlock stays within two of the
 * highest block a child asked for, send the next block, and otherwise jump
 * back to that block. The file record is reached the ROM's way, as MB_FILE
 * reaches it: the work pointer stepped a whole MbParentFile per index, then
 * mFiles[0] from there. */
// @symbol MBi_Calc_SendBlock
void MBi_Calc_SendBlock(int fid) {
    unsigned short now;
    unsigned short req;
    MbParentWork *b = (MbParentWork *)(data_020a9db8 + fid * sizeof(MbParentFile));
    if (b->mFiles[0].mActive == 0)
        return;
    if (b->mFiles[0].mChildBitmap == 0)
        return;
    now = b->mFiles[0].mCurrentBlock;
    req = b->mFiles[0].mRequestedBlock;
    if (req <= now && now <= req + 2) {
        b->mFiles[0].mCurrentBlock++;
        return;
    }
    b->mFiles[0].mCurrentBlock = req;
}

// @symbol MBi_Calc_NextSendBlock
u32 MBi_Calc_NextSendBlock(u32 now, u32 req) {
    if (req <= now)
        req = now;
    return req;
}

// @symbol IsChildAidValid
int IsChildAidValid(u32 aid) {
    if (aid >= 1) {
        if (aid <= 0xf)
            return 1;
    }
    return 0;
}

// @symbol MBi_CommCallParentError
void MBi_CommCallParentError(u16 aid, u16 errcode) {
    MBErrorStatus e;
    e.errcode = errcode;
    MBi_CommChangeParentStateCallbackOnly(aid, MB_COMM_PSTATE_ERROR, &e);
}
}
