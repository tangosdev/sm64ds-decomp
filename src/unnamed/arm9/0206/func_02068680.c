extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern int func_020685cc(int a, void *b, int flag);
extern void MultiStore32Bytes(unsigned val, void *dst, int len);
extern void MultiCopyHalf(int a, void *dst, int n);
extern int func_02068514(int p);

/* NitroSDK MBi_MakeGameInfo(gameInfo, mbGameList, parent): build the
 * multiboot game record a parent beacons from the game's registry entry.
 * func_02067928 builds it into each registered file's slot of the
 * data_020a9db8 work block, with data_020a9db8 + 0x1300 as the parent's
 * user record. The views follow the SDK's MBGameRegistry and MBGameInfo:
 * the whole record is 0x4b8 bytes (the MI_CpuClear16 = MultiStore16
 * length), func_020685cc reads the icon's 0x20-byte palette and 0x200-byte
 * character data from the two path names, and the parent user record is the
 * 0x16-byte MBUserInfo. Only the fields this function touches are spelled
 * out. */
typedef struct MBGameRegistry {
    int romFilePathp;           /* 0x00 */
    int gameNamep;              /* 0x04 -- u16 string */
    int gameIntroductionp;      /* 0x08 -- u16 string */
    int iconCharPathp;          /* 0x0c */
    int iconPalettePathp;       /* 0x10 */
    int ggid;                   /* 0x14 */
    unsigned char maxPlayerNum; /* 0x18 */
} MBGameRegistry;

typedef struct MBGameInfo {
    char icon[0x220];           /* 0x000 -- MBIconInfo: palette, then char data */
    char parent[0x16];          /* 0x220 -- MBUserInfo */
    unsigned char maxPlayerNum; /* 0x236 */
    unsigned char pad_237;
    unsigned short gameName[0x30];         /* 0x238 */
    unsigned short gameIntroduction[0x60]; /* 0x298 */
    unsigned char nowPlayerNum;            /* 0x358 -- volat */
    unsigned char pad_359;
    unsigned short nowPlayerFlag; /* 0x35a */
    char pad_35c[0x14c];
    unsigned short broadcastedPlayerFlag; /* 0x4a8 */
    unsigned char dataAttr;               /* 0x4aa -- 1: icon unreadable, cleared */
    char pad_4ab[5];
    int ggid; /* 0x4b0 */
    char pad_4b4[4];
} MBGameInfo;

#ifndef SM64DS_PLATFORM_PC
typedef char MBGameInfo_size_must_be_0x4b8[sizeof(MBGameInfo) == 0x4b8 ? 1 : -1];
#endif

void func_02068680(char *dst, MBGameRegistry *src, int parent) {
    MBGameInfo *info = (MBGameInfo *)dst;
    volatile unsigned short tmp;
    volatile int z;
    tmp = 0;
    MultiStore16(tmp, dst, 0x4b8);
    info->dataAttr = 0;
    if (func_020685cc(src->iconCharPathp, dst, 1) == 0 |
        func_020685cc(src->iconPalettePathp, dst, 0) == 0) {
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
                  (unsigned)(func_02068514(src->gameNamep) << 0x11) >> 0x10);
    MultiCopyHalf(src->gameIntroductionp, info->gameIntroduction,
                  (unsigned)(func_02068514(src->gameIntroductionp) << 0x11) >> 0x10);
    info->nowPlayerNum = 1;
    info->nowPlayerFlag = 1;
    info->broadcastedPlayerFlag = 1;
}
