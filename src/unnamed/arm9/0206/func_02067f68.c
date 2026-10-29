#include "types.h"

/* NitroSDK MBi_SendVolatBeacon: fill the parent's beacon buffer with the
 * volatile part of the game record being advertised (player count, player
 * flags and up to four child MBUserInfo records not yet broadcast), checksum
 * it and hand it to the beacon setter. The views follow the SDK's
 * MbBeaconSendStatus (data_020a9d2c), MBGameInfo and MbBeacon
 * (data_020a9d40, sent as 0x70 bytes); only the fields this function touches
 * are spelled out.
 *
 * MBGameInfo extends func_0206853c.c's view (fixed part, volatile part,
 * seqNoVolat) with broadcastedPlayerFlag (0x4a8, bit i+1 set as child i is
 * copied, compared against nowPlayerFlag to finish), seqNoFixed (0x4ab),
 * fileNo (0x4ad, six bits in the beacon) and ggid (0x4b0, func_02068680
 * fills it from the registry). The send status's seqNoVolat (0xe) is the
 * copy func_02068188 latches from the record; when it is stale that function
 * restarts the volatile send. beaconNo (0x11) is post-incremented into
 * beacon byte 7 on every send. */
typedef struct MBGameInfo {
    char fixed[0x358];
    unsigned char nowPlayerNum; /* 0x358 -- volat */
    unsigned char pad_359;
    unsigned short nowPlayerFlag;         /* 0x35a -- bit 0 is the parent */
    unsigned short changePlayerFlag;      /* 0x35c */
    char member[15][0x16];                /* 0x35e -- MBUserInfo per child */
    unsigned short broadcastedPlayerFlag; /* 0x4a8 */
    unsigned char dataAttr;               /* 0x4aa */
    unsigned char seqNoFixed;             /* 0x4ab */
    unsigned char seqNoVolat;             /* 0x4ac */
    unsigned char fileNo;                 /* 0x4ad */
    char pad_4ae[2];
    u32 ggid; /* 0x4b0 */
    char pad_4b4[4];
} MBGameInfo;

typedef struct MbBeaconSendStatus {
    MBGameInfo *gameInfoListTop; /* 0x00 */
    MBGameInfo *nowGameInfop;    /* 0x04 */
    u8 *srcp;                    /* 0x08 */
    u8 state;                    /* 0x0c -- 1: record done, 5: volatile send */
    u8 pad_0d;
    u8 seqNoVolat; /* 0x0e */
    u8 pad_0f[2];
    u8 beaconNo; /* 0x11 */
} MbBeaconSendStatus;

typedef struct MbBeacon {
    u32 ggid;        /* 0x00 */
    u8 dataAttr : 2; /* 0x04 -- 2: volatile data */
    u8 fileNo : 6;   /* 0x04 */
    u8 seqNoFixed;   /* 0x05 */
    u8 seqNoVolat;   /* 0x06 */
    u8 beaconNo;     /* 0x07 */
    u16 sum;         /* 0x08 */
    u8 nowPlayerNum; /* 0x0a -- data.volat */
    u8 pad_0b;
    u16 nowPlayerFlag;    /* 0x0c */
    u16 changePlayerFlag; /* 0x0e */
} MbBeacon;

#ifndef SM64DS_PLATFORM_PC
typedef char MBGameInfo_size_must_be_0x4b8[sizeof(MBGameInfo) == 0x4b8 ? 1 : -1];
typedef char MbBeacon_size_must_be_0x10[sizeof(MbBeacon) == 0x10 ? 1 : -1];
#endif

/* data_020a9d2c keeps the u8[] spelling the other MB files use and is read
 * through casts; data_020a9d40 must be the struct object itself: reached
 * through a cast (or a local pointer) the beacon stores reorder. */
extern u8 data_020a9d2c[];
extern MbBeacon data_020a9d40;
extern u16 data_020a9d48[];
extern u8 data_020a9d50[];

extern void func_02068188(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void MultiCopyHalf(void *src, void *dst, int n);
extern u16 func_02067ee4(u16 *p, int len);
extern int func_020688e4(void);
extern int func_020688cc(void);
extern int func_020688b4(void);
extern int func_02068844(void);
extern int func_02063648(int a0, int *src, int len, int a3, int arg5, int arg6, u8 arg7);

void func_02067f68(void)
{
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

    if (((MbBeaconSendStatus *)data_020a9d2c)->seqNoVolat !=
        ((MbBeaconSendStatus *)data_020a9d2c)->nowGameInfop->seqNoVolat) {
        func_02068188();
    }

    ptr = ((MbBeaconSendStatus *)data_020a9d2c)->nowGameInfop;
    data_020a9d40.dataAttr = 2;
    data_020a9d40.seqNoFixed = ptr->seqNoFixed;
    data_020a9d40.seqNoVolat = ((MbBeaconSendStatus *)data_020a9d2c)->seqNoVolat;
    data_020a9d40.ggid = ptr->ggid;
    data_020a9d40.fileNo = ptr->fileNo;
    seq = ((MbBeaconSendStatus *)data_020a9d2c)->beaconNo;
    ((MbBeaconSendStatus *)data_020a9d2c)->beaconNo = seq + 1;
    data_020a9d40.beaconNo = seq;
    data_020a9d40.nowPlayerNum = ptr->nowPlayerNum;
    data_020a9d40.nowPlayerFlag = ptr->nowPlayerFlag;
    data_020a9d40.changePlayerFlag = ptr->changePlayerFlag;

    zero = 0;
    MultiStore16(zero, (char *)data_020a9d50, 0x58);

    ptr = ((MbBeaconSendStatus *)data_020a9d2c)->nowGameInfop;
    mask = ptr->broadcastedPlayerFlag ^ ptr->nowPlayerFlag;
    count = 0;
    dst = data_020a9d50;
    for (i = 0; i < 15; i++) {
        if (mask & (2 << i)) {
            MultiCopyHalf(((MbBeaconSendStatus *)data_020a9d2c)->nowGameInfop->member[i], dst,
                          0x16);
            count++;
            dst += 0x16;
            ((MbBeaconSendStatus *)data_020a9d2c)->nowGameInfop->broadcastedPlayerFlag |= 2 << i;
            if (count == 4) {
                break;
            }
        }
    }

    if (count < 4) {
        data_020a9d50[count * 0x16] &= ~0xf0;
    }

    data_020a9d40.sum = 0;
    data_020a9d40.sum = func_02067ee4(data_020a9d48, 0x68);

    ptr = ((MbBeaconSendStatus *)data_020a9d2c)->nowGameInfop;
    if (ptr->broadcastedPlayerFlag == ptr->nowPlayerFlag) {
        ((MbBeaconSendStatus *)data_020a9d2c)->state = 1;
    }

    a = func_020688e4();
    b = func_020688cc();
    c = func_020688b4();
    func_02063648(0, (int *)&data_020a9d40, 0x70, a, b, c, func_02068844() | 2);
}
