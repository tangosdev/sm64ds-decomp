/* mb_gameinfo.c: the NitroSDK multiboot parent's game records and the
 * beacon that advertises them. A game record (MBGameInfo, 0x4b8 bytes) is
 * built from the game's registry entry (MBi_MakeGameInfo) and chained on
 * the send status's list (MB_AddGameInfo). Each BEACON_SENT event runs the
 * send state machine (MB_SendGameInfoBeacon). For the current record it
 * sends the fixed part in 0x62-byte pieces (MBi_InitSendFixedBeacon,
 * MBi_SendFixedBeacon), then the volatile part, which is the player count,
 * the player flags and up to four child records per beacon
 * (MBi_InitSendVolatBeacon, MBi_SendVolatBeacon). Then it moves to the
 * next record (MBi_ReadyBeaconSendStatus).
 *
 * Why these thirteen functions are one file:
 * - NitroSDK's mb_gameinfo.c (ntrtwl's copy; Pokemon D/P's agrees) defines
 *   its parent-side functions as MBi_MakeGameInfo, MBi_ReadIconInfo,
 *   MB_UpdateGameInfoMember, mystrlen, MB_AddGameInfo, MB_DeleteGameInfo,
 *   MB_InitSendGameInfoStatus, MBi_ClearSendStatus, MB_SendGameInfoBeacon,
 *   MBi_ReadyBeaconSendStatus, MBi_InitSendFixedBeacon,
 *   MBi_SendFixedBeacon, MBi_InitSendVolatBeacon, MBi_SendVolatBeacon.
 *   mwccarm emits .text in reverse source order, and 0x02067f68..0x0206879c
 *   is that list, less MB_DeleteGameInfo, from MBi_SendVolatBeacon up to
 *   MBi_MakeGameInfo, read top to bottom. This file keeps the source order.
 *   Lego Star Wars II's symbols.txt has the same thirteen names in the
 *   same order (0x020daa6c..0x020db2a8).
 * - The send status data_020a9d2c and the beacon buffer data_020a9d40
 *   are used by these functions and by no other code.
 * - Bottom edge: func_02067f2c (cache flush, then a DMA copy) is not game
 *   record code. The SDK file's other functions (MB_DeleteGameInfo and the
 *   child's beacon-receive side) are not in this ROM, nor in Lego Star
 *   Wars II's: nothing calls them, so they were dead-stripped. Top edge:
 *   arm9/MB_WmBase begins at 0x0206879c.
 *
 * This SDK is older than ntrtwl's, so its record is 8 bytes shorter
 * (no userVolatData) and has no send-volat callback. MB_SendGameInfoBeacon
 * and the two send steps take no ggid/tgid/attribute; the send steps fetch
 * them themselves. The field names follow the SDK's MBGameInfo,
 * MbBeaconSendStatus and MbBeacon. The reference places a few of them
 * differently, and where it does, the existing names are kept.
 */
#include "types.h"

/* local extern: MI_Memory.c's asm fills and copies; no header declares them. */
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern void MultiCopyHalf(const void *src, void *dst, s32 size);
extern void MultiStore32Bytes(unsigned val, void *dst, int len);
extern void FS_InitFile(int *s);
extern int func_0205d518(void *file, const char *path);
extern int FS_CloseFile(int *s);
extern int FS_ReadFile(int *s, int a, int b);
extern u16 func_02067ee4(u16 *p, int len);
extern int func_020688e4(void);
extern int func_020688cc(void);
extern int MBi_GetTgid(void);
extern int MBi_GetAttribute(void);
extern int WM_SetGameInfo(int a0, int *src, int len, int a3, int arg5, int arg6, u8 arg7);

typedef struct MBGameRegistry {
    const char *romFilePathp;     /* 0x00 */
    const u16 *gameNamep;         /* 0x04 */
    const u16 *gameIntroductionp; /* 0x08 */
    const char *iconCharPathp;    /* 0x0c */
    const char *iconPalettePathp; /* 0x10 */
    int ggid;                     /* 0x14 */
    unsigned char maxPlayerNum;   /* 0x18 */
} MBGameRegistry;

typedef struct MBGameInfo {
    /* fixed part, sent in 0x62-byte pieces */
    char icon[0x220];           /* 0x000 -- MBIconInfo: palette, then char data */
    char parent[0x16];          /* 0x220 -- MBUserInfo */
    unsigned char maxPlayerNum; /* 0x236 */
    unsigned char pad_237;
    unsigned short gameName[0x30];         /* 0x238 */
    unsigned short gameIntroduction[0x60]; /* 0x298 */
    /* volatile part */
    unsigned char nowPlayerNum; /* 0x358 */
    unsigned char pad_359;
    unsigned short nowPlayerFlag;         /* 0x35a -- bit 0 is the parent */
    unsigned short changePlayerFlag;      /* 0x35c */
    char member[15][0x16];                /* 0x35e -- MBUserInfo per child */
    unsigned short broadcastedPlayerFlag; /* 0x4a8 -- bit i+1 set as child i is sent */
    unsigned char dataAttr;               /* 0x4aa -- 1: icon unreadable, cleared */
    unsigned char seqNoFixed;             /* 0x4ab */
    unsigned char seqNoVolat;             /* 0x4ac -- bumped by MB_UpdateGameInfoMember */
    unsigned char fileNo;                 /* 0x4ad -- six bits in the beacon */
    char pad_4ae[2];
    u32 ggid;                 /* 0x4b0 */
    struct MBGameInfo *nextp; /* 0x4b4 */
} MBGameInfo;

typedef struct MbBeaconSendStatus {
    MBGameInfo *gameInfoListTop; /* 0x00 */
    MBGameInfo *nowGameInfop;    /* 0x04 */
    u8 *srcp;                    /* 0x08 -- next fixed-part piece */
    u8 state;                    /* 0x0c */
    u8 seqNoFixed;               /* 0x0d */
    u8 seqNoVolat;               /* 0x0e -- latched from the record */
    u8 fragmentNo;               /* 0x0f */
    u8 fragmentMaxNum;           /* 0x10 */
    u8 beaconNo;                 /* 0x11 */
} MbBeaconSendStatus;

typedef struct MbBeacon {
    u32 ggid;        /* 0x00 */
    u8 dataAttr : 2; /* 0x04 -- 2: volatile data */
    u8 fileNo : 6;   /* 0x04 */
    u8 seqNoFixed;   /* 0x05 */
    u8 seqNoVolat;   /* 0x06 */
    u8 beaconNo;     /* 0x07 */
    union {
        struct {
            u16 sum;           /* 0x08 */
            u8 fragmentNo;     /* 0x0a */
            u8 fragmentMaxNum; /* 0x0b */
            u8 size;           /* 0x0c */
            u8 rsv;            /* 0x0d */
            u8 data[0x62];     /* 0x0e */
        } fixed;
        struct {
            u16 sum;         /* 0x08 */
            u8 nowPlayerNum; /* 0x0a */
            u8 pad;
            u16 nowPlayerFlag;    /* 0x0c */
            u16 changePlayerFlag; /* 0x0e */
            u8 member[4][0x16];   /* 0x10 */
        } volat;
    } data;
} MbBeacon;

#ifndef SM64DS_PLATFORM_PC
typedef char MBGameInfo_size_must_be_0x4b8[sizeof(MBGameInfo) == 0x4b8 ? 1 : -1];
typedef char MbBeacon_size_must_be_0x70[sizeof(MbBeacon) == 0x70 ? 1 : -1];
#endif

extern MbBeaconSendStatus data_020a9d2c;
/* data_020a9d40 must be the struct object itself: reached through a cast
 * (or a local pointer) the beacon stores reorder. The checksum span
 * (data, +8), the fixed piece (+0xe) and the volatile members (+0x10) are
 * addressed through their own symbols, each with its own literal-pool word. */
extern MbBeacon data_020a9d40;
extern u16 data_020a9d48[];
extern u8 data_020a9d4e[];
extern u8 data_020a9d50[];

void MBi_MakeGameInfo(char *dst, MBGameRegistry *src, const void *parent);
int MBi_ReadIconInfo(const char *filePathp, char *iconp, int char_flag);
void MB_UpdateGameInfoMember(char *self, void *src, unsigned int flags, int s);
int mystrlen(const u16 *str);
void MB_AddGameInfo(MBGameInfo *n);
void MB_InitSendGameInfoStatus(void);
void MBi_ClearSendStatus(void);
void MB_SendGameInfoBeacon(void);
int MBi_ReadyBeaconSendStatus(void);
void MBi_InitSendFixedBeacon(void);
void MBi_SendFixedBeacon(void);
void MBi_InitSendVolatBeacon(void);
void MBi_SendVolatBeacon(void);

// @symbol MBi_MakeGameInfo
/* NitroSDK MBi_MakeGameInfo(gameInfo, mbGameList, parent): build the game
 * record a parent beacons from the game's registry entry. func_02067928
 * builds it into each registered file's slot of the data_020a9db8 work
 * block, with data_020a9db8 + 0x1300 as the parent's user record.
 * MBi_ReadIconInfo reads the icon's 0x20-byte palette and 0x200-byte
 * character data from the two path names. One argument differs from newer
 * SDKs, which copy the introduction as sizeof(gameIntroduction): here it
 * is sized by mystrlen, like the game name. */
void MBi_MakeGameInfo(char *dst, MBGameRegistry *src, const void *parent) {
    MBGameInfo *info = (MBGameInfo *)dst;
    volatile unsigned short tmp;
    volatile int z;
    tmp = 0;
    MultiStore16(tmp, dst, 0x4b8);
    info->dataAttr = 0;
    if (MBi_ReadIconInfo(src->iconCharPathp, dst, 1) == 0 |
        MBi_ReadIconInfo(src->iconPalettePathp, dst, 0) == 0) {
        info->dataAttr = 1;
        z = 0;
        MultiStore32Bytes(z, dst, 0x220);
    }
    info->ggid = src->ggid;
    if (parent) {
        MultiCopyHalf(parent, info->parent, 0x16);
    }
    info->maxPlayerNum = src->maxPlayerNum;
    MultiCopyHalf(src->gameNamep, info->gameName,
                  (unsigned)(mystrlen(src->gameNamep) << 0x11) >> 0x10);
    MultiCopyHalf(src->gameIntroductionp, info->gameIntroduction,
                  (unsigned)(mystrlen(src->gameIntroductionp) << 0x11) >> 0x10);
    info->nowPlayerNum = 1;
    info->nowPlayerFlag = 1;
    info->broadcastedPlayerFlag = 1;
}

// @symbol MBi_ReadIconInfo
/* NitroSDK MBi_ReadIconInfo: read one icon file into a game record. With
 * char_flag set it reads 0x200 bytes of character data to iconp + 0x20,
 * otherwise 0x20 bytes of palette to iconp. It fails (0) on a null path, a
 * failed open, or a file whose length (bottom - top, words 9 and 8 of the
 * FSFile) is not exactly that size. Newer SDKs open with
 * FS_OpenFileEx(file, path, FS_FILEMODE_R); this one takes the
 * two-argument open func_0205d518. */
int MBi_ReadIconInfo(const char *filePathp, char *iconp, int char_flag) {
    int local[0x12];
    int sz;
    int param;

    sz = char_flag ? 0x200 : 0x20;
    if (char_flag)
        iconp += 0x20;

    if (filePathp == 0)
        return 0;

    FS_InitFile(local);

    if (func_0205d518(local, filePathp) == 0)
        return 0;

    param = local[9] - local[8];
    if (sz != param) {
        FS_CloseFile(local);
        return 0;
    }

    FS_ReadFile(local, (int)iconp, sz);
    FS_CloseFile(local);
    return 1;
}

// @symbol MB_UpdateGameInfoMember
/* NitroSDK MB_UpdateGameInfoMember(gameInfo, member, nowPlayerFlag,
 * changePlayerFlag): copy the fifteen child MBUserInfo records (0x14a bytes
 * in all) into the record's volatile part, count the children in
 * nowPlayerFlag (bits 1..15) plus the parent into nowPlayerNum, store the
 * flags with the parent's bit 0 set, and bump seqNoVolat so the beacon
 * sends the volatile part again. MBi_CommParentCallback calls it for each
 * registered file whose entries changed. */
void MB_UpdateGameInfoMember(char *self, void *src, unsigned int flags, int s) {
    MBGameInfo *gameInfo = (MBGameInfo *)self;
    int i;
    int cnt;
    cnt = 1;
    MultiCopyHalf(src, gameInfo->member, 0x14a);
    for (i = 0; i < 0xf; i++) {
        if (flags & (2 << i))
            cnt = (cnt + 1) & 0xff;
    }
    gameInfo->nowPlayerNum = (unsigned char)cnt;
    gameInfo->nowPlayerFlag = (short)(flags | 1);
    gameInfo->changePlayerFlag = (short)s;
    gameInfo->seqNoVolat += 1;
}

// @symbol mystrlen
/* NitroSDK mystrlen: length of a NUL-terminated u16 string, in characters.
 * MBi_MakeGameInfo sizes the game name and introduction with it. */
int mystrlen(const u16 *str) {
    int count = 0;

    while (*str++ != 0) {
        count++;
    }
    return count;
}

// @symbol MB_AddGameInfo
/* NitroSDK MB_AddGameInfo: append a game record to the beacon list. The new
 * record ends the list. func_02067928 adds each registered file's record
 * this way. */
void MB_AddGameInfo(MBGameInfo *n) {
    MBGameInfo *p = data_020a9d2c.gameInfoListTop;
    if (p == 0) {
        data_020a9d2c.gameInfoListTop = n;
    } else {
        while (p->nextp != 0) {
            p = p->nextp;
        }
        p->nextp = n;
    }
    n->nextp = 0;
}

// @symbol MB_InitSendGameInfoStatus
/* NitroSDK MB_InitSendGameInfoStatus: reset the beacon send status. It
 * empties the record list, clears the current record, sets state 1 (pick a
 * record) and clears the send counters. The parent setup func_02068ae8
 * calls it after it clears the file table.
 *
 * Later SDKs have one more statement, `sSendVolatCallback = NULL;`. It
 * clears the hook that MB_SetSendVolatCallback installs. This SDK has no
 * such hook, because its MBi_SendVolatBeacon calls no callback, so there is
 * nothing to clear. */
void MB_InitSendGameInfoStatus(void) {
    data_020a9d2c.gameInfoListTop = 0;
    data_020a9d2c.nowGameInfop = 0;
    data_020a9d2c.state = 1;
    MBi_ClearSendStatus();
}

// @symbol MBi_ClearSendStatus
/* NitroSDK MBi_ClearSendStatus: clear the per-record send counters, bytes
 * 0xd..0x11 of the send status. Called when MBi_ReadyBeaconSendStatus picks the next
 * record and from MB_InitSendGameInfoStatus. */
void MBi_ClearSendStatus(void) {
    data_020a9d2c.seqNoFixed = 0;
    data_020a9d2c.seqNoVolat = 0;
    data_020a9d2c.fragmentNo = 0;
    data_020a9d2c.fragmentMaxNum = 0;
    data_020a9d2c.beaconNo = 0;
}

// @symbol MB_SendGameInfoBeacon
/* NitroSDK MB_SendGameInfoBeacon: run the beacon send state machine until
 * a step sets a beacon or there is nothing to send. Newer SDKs pass ggid,
 * tgid and attr down to the two send steps. In this one MBi_SendFixedBeacon
 * and MBi_SendVolatBeacon fetch them themselves, so the function takes no
 * parameters. The parent's event handler MBi_CommParentCallback calls it
 * on each BEACON_SENT (0x1c). */
void MB_SendGameInfoBeacon(void) {
    for (;;) {
        switch (data_020a9d2c.state) {
        case 0:
        case 1:
            if (MBi_ReadyBeaconSendStatus() != 0)
                break;
            return;
        case 2:
            MBi_InitSendFixedBeacon();
            break;
        case 3:
            MBi_SendFixedBeacon();
            return;
        case 4:
            MBi_InitSendVolatBeacon();
            break;
        case 5:
            MBi_SendVolatBeacon();
            return;
        case 6:
            break;
        }
    }
}

// @symbol MBi_ReadyBeaconSendStatus
/* Pick the next record to advertise: the one after the current record, or
 * the list's first when there is no current record or it is the last.
 * Then latch its seqNoVolat and move to state 2. Returns 0 with an empty
 * list.
 *
 * NitroSDK MBi_ReadyBeaconSendStatus as it was before Rev 1.42
 * (11/18/2005, "fix MBi_ReadyBeaconSendStatus()"). That revision added the
 * empty-list WM_SetGameInfo(NULL, bsendBuff, sizeof(MbBeacon),
 * MBi_GetGgid(), MBi_GetTgid(), WM_ATTR_FLAG_CS) that ntrtwl, Pokemon D/P
 * and HeartGold carry; this one only returns 0. Rev 1.13 (09/2004) already
 * latched seqNoVolat here, as this body does. Formerly func_02068398.
 *
 * The fallback is written as the `||` arm of an if/else. Both failed tests
 * then reach `res = sel`, a two-predecessor block that mwccarm will not
 * predicate, and once `res` and `sel` share r1 that copy is gone, leaving
 * the ROM's `beq` over a lone `mov r1, r0`. A plain `if (q != 0) sel = q;`
 * predicates into `movne` instead (notes/mwccarm-codegen.md 6u). */
int MBi_ReadyBeaconSendStatus(void) {
    MBGameInfo *sel = data_020a9d2c.gameInfoListTop;
    MBGameInfo *p;
    MBGameInfo *res;

    if (sel == 0)
        return 0;

    p = data_020a9d2c.nowGameInfop;
    if (p == 0 || (p = p->nextp) == 0)
        res = sel;
    else
        res = p;
    data_020a9d2c.nowGameInfop = res;

    MBi_ClearSendStatus();
    data_020a9d2c.seqNoVolat = data_020a9d2c.nowGameInfop->seqNoVolat;
    data_020a9d2c.state = 2;
    return 1;
}

// @symbol MBi_InitSendFixedBeacon
/* NitroSDK MBi_InitSendFixedBeacon, beacon send state 2: set up the fixed
 * part of the current record. With dataAttr zero the send starts at the
 * record's first byte and takes 9 beacons. Otherwise it starts past the
 * 0x220-byte icon and takes 4. Each beacon carries 0x62 bytes
 * (MBi_SendFixedBeacon). Then go to state 3. */
void MBi_InitSendFixedBeacon(void) {
    MBGameInfo *p;
    if (data_020a9d2c.state != 2)
        return;
    p = data_020a9d2c.nowGameInfop;
    if (p->dataAttr == 0) {
        data_020a9d2c.fragmentMaxNum = 9;
        data_020a9d2c.srcp = (u8 *)p;
    } else {
        data_020a9d2c.fragmentMaxNum = 4;
        data_020a9d2c.srcp = (u8 *)p + 0x220;
    }
    data_020a9d2c.state = 3;
}

// @symbol MBi_SendFixedBeacon
/* NitroSDK MBi_SendFixedBeacon, beacon send state 3: copy the next 0x62-byte
 * piece of the record's fixed part into the beacon, zero-filling a short
 * last piece, stamp the record's attributes and the checksum, and set the
 * beacon. After the last piece go to state 4. Newer SDKs take ggid, tgid
 * and attr as parameters. This one reads them itself (func_020688e4,
 * func_020688cc, MBi_GetTgid, MBi_GetAttribute). */
void MBi_SendFixedBeacon(void) {
    volatile u16 v;

    {
        u8 *p = (u8 *)data_020a9d2c.nowGameInfop;
        u8 *cur = data_020a9d2c.srcp;
        unsigned lim = (unsigned)p + 0x358;
        unsigned nxt = (unsigned)cur + 0x62;
        if (nxt <= lim) {
            data_020a9d40.data.fixed.size = 0x62;
        } else {
            data_020a9d40.data.fixed.size = (u8)(lim - (unsigned)cur);
            v = 0;
            MultiStore16(v, (char *)data_020a9d4e + data_020a9d40.data.fixed.size,
                         0x62 - data_020a9d40.data.fixed.size);
        }
    }
    MultiCopyHalf(data_020a9d2c.srcp, data_020a9d4e, data_020a9d40.data.fixed.size);

    data_020a9d40.data.fixed.fragmentNo = data_020a9d2c.fragmentNo;
    data_020a9d40.data.fixed.fragmentMaxNum = data_020a9d2c.fragmentMaxNum;
    data_020a9d40.dataAttr = data_020a9d2c.nowGameInfop->dataAttr;
    data_020a9d40.seqNoFixed = data_020a9d2c.nowGameInfop->seqNoFixed;
    data_020a9d40.seqNoVolat = data_020a9d2c.seqNoVolat;
    data_020a9d40.ggid = data_020a9d2c.nowGameInfop->ggid;
    data_020a9d40.fileNo = data_020a9d2c.nowGameInfop->fileNo;
    data_020a9d40.beaconNo = data_020a9d2c.beaconNo++;
    data_020a9d40.data.fixed.sum = 0;
    data_020a9d40.data.fixed.sum = func_02067ee4(data_020a9d48, 0x68);

    if (++data_020a9d2c.fragmentNo < data_020a9d2c.fragmentMaxNum)
        data_020a9d2c.srcp += 0x62;
    else
        data_020a9d2c.state = 4;

    WM_SetGameInfo(0, (int *)&data_020a9d40, 0x70, func_020688e4(), func_020688cc(), MBi_GetTgid(),
                   MBi_GetAttribute() | 2);
}

// @symbol MBi_InitSendVolatBeacon
/* NitroSDK MBi_InitSendVolatBeacon, beacon send state 4: start the volatile
 * part of the current record. Mark the parent's bit in its
 * broadcastedPlayerFlag, latch its seqNoVolat into the send status, and go
 * to state 5. The state loop calls it, and MBi_SendVolatBeacon calls it again
 * when that latched copy goes stale. */
void MBi_InitSendVolatBeacon(void) {
    data_020a9d2c.nowGameInfop->broadcastedPlayerFlag = 1;
    data_020a9d2c.seqNoVolat = data_020a9d2c.nowGameInfop->seqNoVolat;
    data_020a9d2c.state = 5;
}

// @symbol MBi_SendVolatBeacon
/* NitroSDK MBi_SendVolatBeacon, beacon send state 5: fill the beacon with
 * the record's volatile part (player count, player flags and up to four
 * child records not yet broadcast), checksum it and set it. When every
 * child has been sent, go back to state 1. Statement for statement it is
 * ntrtwl's NitroSDK body, less what this SDK lacks: the send-volat callback
 * before and after, and the eight userVolatData bytes. Like
 * MBi_SendFixedBeacon it fetches ggid, tgid and attribute itself and sets
 * attribute | 2 where ntrtwl's passes them in and sets | 3 (the ENTRY
 * flag, added in Rev 1.41). */
void MBi_SendVolatBeacon(void) {
    MBGameInfo *ptr;
    volatile u16 zero;
    u16 mask;
    int i;
    u32 count;
    u8 *dst;
    int a;
    int b;
    int c;
    u8 seq;

    if (data_020a9d2c.seqNoVolat != data_020a9d2c.nowGameInfop->seqNoVolat) {
        MBi_InitSendVolatBeacon();
    }

    ptr = data_020a9d2c.nowGameInfop;
    data_020a9d40.dataAttr = 2;
    data_020a9d40.seqNoFixed = ptr->seqNoFixed;
    data_020a9d40.seqNoVolat = data_020a9d2c.seqNoVolat;
    data_020a9d40.ggid = ptr->ggid;
    data_020a9d40.fileNo = ptr->fileNo;
    seq = data_020a9d2c.beaconNo;
    data_020a9d2c.beaconNo = seq + 1;
    data_020a9d40.beaconNo = seq;
    data_020a9d40.data.volat.nowPlayerNum = ptr->nowPlayerNum;
    data_020a9d40.data.volat.nowPlayerFlag = ptr->nowPlayerFlag;
    data_020a9d40.data.volat.changePlayerFlag = ptr->changePlayerFlag;

    zero = 0;
    MultiStore16(zero, (char *)data_020a9d50, 0x58);

    ptr = data_020a9d2c.nowGameInfop;
    mask = ptr->broadcastedPlayerFlag ^ ptr->nowPlayerFlag;
    count = 0;
    dst = data_020a9d50;
    for (i = 0; i < 15; i++) {
        if (mask & (2 << i)) {
            MultiCopyHalf(data_020a9d2c.nowGameInfop->member[i], dst, 0x16);
            count++;
            dst += 0x16;
            data_020a9d2c.nowGameInfop->broadcastedPlayerFlag |= 2 << i;
            if (count == 4) {
                break;
            }
        }
    }

    if (count < 4) {
        data_020a9d50[count * 0x16] &= ~0xf0;
    }

    data_020a9d40.data.volat.sum = 0;
    data_020a9d40.data.volat.sum = func_02067ee4(data_020a9d48, 0x68);

    ptr = data_020a9d2c.nowGameInfop;
    if (ptr->broadcastedPlayerFlag == ptr->nowPlayerFlag) {
        data_020a9d2c.state = 1;
    }

    a = func_020688e4();
    b = func_020688cc();
    c = MBi_GetTgid();
    WM_SetGameInfo(0, (int *)&data_020a9d40, 0x70, a, b, c, MBi_GetAttribute() | 2);
}
