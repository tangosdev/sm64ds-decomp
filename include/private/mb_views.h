#ifndef PRIVATE_MB_VIEWS_H
#define PRIVATE_MB_VIEWS_H

/* The 0x16-byte player record sent by the multiboot parent. The 2004 ROM
 * reads the name length at +1 and ten UTF-16 code units from +2; the later
 * NitroSDK names the record MBUserInfo. The upper nibble is written here but
 * its meaning in this SDK revision is not established. */
typedef struct MBUserInfo {
    unsigned char favoriteColor : 4;
    unsigned char playerNo : 4;
    unsigned char nameLength;
    unsigned short name[10];
} MBUserInfo;

typedef char MBUserInfo_size_must_be_0x16[sizeof(MBUserInfo) == 0x16 ? 1 : -1];

/* NitroSDK MBGameRegistry: the program a parent offers, as the game passes
 * it to MB_RegisterFile. The parent checks a child's ggid and maxPlayerNum
 * against it (MBi_CommParentRecvDataPerChild). */
typedef struct MBGameRegistry {
    const char *romFilePathp;                /* 0x00 */
    const unsigned short *gameNamep;         /* 0x04 */
    const unsigned short *gameIntroductionp; /* 0x08 */
    const char *iconCharPathp;               /* 0x0c */
    const char *iconPalettePathp;            /* 0x10 */
    int ggid;                                /* 0x14 */
    unsigned char maxPlayerNum;              /* 0x18 */
} MBGameRegistry;

/* The NitroSDK multiboot game record (MBGameInfo, 0x4b8 bytes) as the ROM's
 * code reads it: built by MBi_MakeGameInfo, chained by MB_AddGameInfo and
 * sent by the beacon state machine (src/engine/mb/MB_GameInfo.c); the
 * parent's file registration (MB_RegisterFile) fills fileNo and seqNoFixed in
 * the copy at MbParentFile::game_info. Field names follow the SDK's
 * mb.h; unread bytes stay padding. Plain C types so C and C++ files can
 * include it without types.h. */
typedef struct MBGameInfoFixed {
    char icon[0x220];           /* 0x000 -- MBIconInfo: palette, then char data */
    MBUserInfo parent;          /* 0x220 -- one MBUserInfo */
    unsigned char maxPlayerNum; /* 0x236 */
    unsigned char pad_237;
    unsigned short gameName[0x30];         /* 0x238 */
    unsigned short gameIntroduction[0x60]; /* 0x298 */
} MBGameInfoFixed;

typedef struct MBGameInfoVolatile {
    unsigned char nowPlayerNum; /* 0x358 */
    unsigned char pad_359;
    unsigned short nowPlayerFlag;         /* 0x35a -- bit 0 is the parent */
    unsigned short changePlayerFlag;      /* 0x35c */
    MBUserInfo member[15];                /* 0x35e -- one MBUserInfo per child */
    unsigned short broadcastedPlayerFlag; /* 0x4a8 -- bit i+1 set as child i is sent */
} MBGameInfoVolatile;

typedef struct MBGameInfo {
    MBGameInfoFixed fixed;    /* 0x000 -- sent in 0x62-byte pieces */
    MBGameInfoVolatile volat; /* 0x358 */
    unsigned char dataAttr;   /* 0x4aa -- 1: icon unreadable, cleared */
    unsigned char seqNoFixed; /* 0x4ab */
    unsigned char seqNoVolat; /* 0x4ac -- bumped by MB_UpdateGameInfoMember */
    unsigned char fileNo;     /* 0x4ad -- six bits in the beacon */
    char pad_4ae[2];
    unsigned int ggid;        /* 0x4b0 */
    struct MBGameInfo *nextp; /* 0x4b4 */
} MBGameInfo;

#ifndef SM64DS_PLATFORM_PC
typedef char MBGameInfo_size_must_be_0x4b8[sizeof(MBGameInfo) == 0x4b8 ? 1 : -1];
#endif

#endif
