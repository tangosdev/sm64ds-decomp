/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x0208fb94..0x0208fd74, 480 bytes, 40 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208fd5c: 12 bytes. */
char data_0208fd5c[12] = "C0_SWITCH";

/* 0x0208fd50: 12 bytes. */
char data_0208fd50[12] = "PATH_LIFT";

/* 0x0208fd44: 12 bytes. */
char data_0208fd44[12] = "HS_B_STAR";

/* 0x0208fd38: 12 bytes. */
char data_0208fd38[12] = "HANSWITCH";

/* 0x0208fd2c: 12 bytes. */
char data_0208fd2c[12] = "YOSHI_EGG";

/* 0x0208fd20: 12 bytes. */
char data_0208fd20[12] = "GAME_OVER";

/* 0x0208fd14: 12 bytes. */
char data_0208fd14[12] = "MG_MCARLO";

/* 0x0208fd08: 12 bytes. */
char data_0208fd08[12] = "WATERFALL";

/* 0x0208fcfc: 12 bytes. */
char data_0208fcfc[12] = "ICE_BOARD";

/* 0x0208fcf0: 12 bytes. */
char data_0208fcf0[12] = "MG_TERESA";

/* 0x0208fce4: 12 bytes. */
char data_0208fce4[12] = "PUSHBLOCK";

/* 0x0208fcd8: 12 bytes. */
char data_0208fcd8[12] = "MG_FLOWER";

/* 0x0208fccc: 12 bytes. */
char data_0208fccc[12] = "OBJ_ABUKU";

/* 0x0208fcc0: 12 bytes. */
char data_0208fcc0[12] = "RACE_NOKO";

/* 0x0208fcb4: 12 bytes. */
char data_0208fcb4[12] = "BLUE_COIN";

/* 0x0208fca8: 12 bytes. */
char data_0208fca8[12] = "PL_CLOSET";

/* 0x0208fc9c: 12 bytes. */
char data_0208fc9c[12] = "LAST_STAR";

/* 0x0208fc90: 12 bytes. */
char data_0208fc90[12] = "EWB_ICE_C";

/* 0x0208fc84: 12 bytes. */
char data_0208fc84[12] = "EWB_ICE_B";

/* 0x0208fc78: 12 bytes. */
char data_0208fc78[12] = "DOSSY_CAP";

/* 0x0208fc6c: 12 bytes. */
char data_0208fc6c[12] = "KOOPAFIRE";

/* 0x0208fc60: 12 bytes. */
char data_0208fc60[12] = "BUTTERFLY";

/* 0x0208fc54: 12 bytes. */
char data_0208fc54[12] = "BATANKING";

/* 0x0208fc48: 12 bytes. */
char data_0208fc48[12] = "KOOPATAIL";

/* 0x0208fc3c: 12 bytes. */
char data_0208fc3c[12] = "TERESAPIT";

/* 0x0208fc30: 12 bytes. */
char data_0208fc30[12] = "BOOKSHELF";

/* 0x0208fc24: 12 bytes. */
char data_0208fc24[12] = "FIREPAKUN";

/* 0x0208fc18: 12 bytes. */
char data_0208fc18[12] = "MG_MEMORY";

/* 0x0208fc0c: 12 bytes. */
char data_0208fc0c[12] = "KM2_AGARU";

/* 0x0208fc00: 12 bytes. */
char data_0208fc00[12] = "MG_SLOT1";

/* 0x0208fbf4: 12 bytes. */
char data_0208fbf4[12] = "FL_BLOCK";

/* 0x0208fbe8: 12 bytes. */
char data_0208fbe8[12] = "BLOCK_LL";

/* 0x0208fbdc: 12 bytes. */
char data_0208fbdc[12] = "KM2_GURA";

/* 0x0208fbd0: 12 bytes. */
char data_0208fbd0[12] = "BK_TOWER";

/* 0x0208fbc4: 12 bytes. */
char data_0208fbc4[12] = "MG_LUIGI";

/* 0x0208fbb8: 12 bytes. */
char data_0208fbb8[12] = "MG_3DESP";

/* 0x0208fbac: 12 bytes. */
char data_0208fbac[12] = "KURIBO_L";

/* 0x0208fba0: 12 bytes. */
char data_0208fba0[12] = "MG_JUMP2";

/* 0x0208fb94: 12 bytes. */
char data_0208fb94[12] = "OBJ_KUMO";

/* 0x0208fd68: 12 bytes. */
char data_0208fd68[12] = "C1_HIKARI";
