extern void MultiCopyHalf(void *dst, void *src, int n);

/* NitroSDK MB_UpdateGameInfoMember(gameInfo, member, nowPlayerFlag,
 * changePlayerFlag): copy the fifteen child MBUserInfo records (0x16 bytes
 * each, 0x14a in all) into the game record's volatile part, count the
 * children in nowPlayerFlag (bits 1..15) plus the parent into nowPlayerNum,
 * store the flags with the parent's bit 0 set, and bump seqNoVolat so the
 * beacon (func_02068188, func_02067f68) sends the volatile part again.
 * func_02066a94 calls it for each registered file whose entries changed.
 * The view extends func_02068680.c's MBGameInfo with changePlayerFlag,
 * member and seqNoVolat, which only this function writes; only the fields
 * it touches are spelled out. */
typedef struct MBGameInfo {
    char fixed[0x358];
    unsigned char nowPlayerNum; /* 0x358 -- volat */
    unsigned char pad_359;
    unsigned short nowPlayerFlag;    /* 0x35a -- bit 0 is the parent */
    unsigned short changePlayerFlag; /* 0x35c */
    char member[15][0x16];           /* 0x35e -- MBUserInfo per child */
    char pad_4a8[4];                 /* broadcastedPlayerFlag, dataAttr, ... */
    unsigned char seqNoVolat;        /* 0x4ac -- the beacon compares it */
} MBGameInfo;

void func_0206853c(char *self, void *src, unsigned int flags, int s)
{
    MBGameInfo *gameInfo = (MBGameInfo *)self;
    int i;
    int cnt;
    cnt = 1;
    MultiCopyHalf(src, gameInfo->member, 0x14a);
    for (i = 0; i < 0xf; i++) {
        if (flags & (2 << i)) cnt = (cnt + 1) & 0xff;
    }
    gameInfo->nowPlayerNum = (unsigned char)cnt;
    gameInfo->nowPlayerFlag = (short)(flags | 1);
    gameInfo->changePlayerFlag = (short)s;
    gameInfo->seqNoVolat += 1;
}
