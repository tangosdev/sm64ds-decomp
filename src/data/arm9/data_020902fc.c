/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x020902fc..0x02090538, 572 bytes, 40 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x02090468: 16 bytes. */
char data_02090468[16] = "BK_BILLBOARD";

/* 0x02090508: 16 bytes. */
char data_02090508[16] = "PENGUIN_BABY";

/* 0x020904f8: 16 bytes. */
char data_020904f8[16] = "ENEMY_CREATE";

/* 0x020904a8: 16 bytes. */
char data_020904a8[16] = "FL_UKIYUKA_L";

/* 0x020904b8: 16 bytes. */
char data_020904b8[16] = "OBJ_RED_FIRE";

/* 0x020904c8: 16 bytes. */
char data_020904c8[16] = "WL_SUBMARINE";

/* 0x020904d8: 16 bytes. */
char data_020904d8[16] = "MG_CURLING_J";

/* 0x020904e8: 16 bytes. */
char data_020904e8[16] = "ENEMY_SWITCH";

/* 0x02090518: 16 bytes. */
char data_02090518[16] = "MG_SMARTBALL";

/* 0x02090488: 16 bytes. */
char data_02090488[16] = "PICTURE_GATE";

/* 0x020903c8: 16 bytes. */
char data_020903c8[16] = "MONKEY_THIEF";

/* 0x020903d8: 16 bytes. */
char data_020903d8[16] = "KM2_UKISHIMA";

/* 0x020903e8: 16 bytes. */
char data_020903e8[16] = "KERONPA_FIRE";

/* 0x02090448: 16 bytes. */
char data_02090448[16] = "HATENA_BLOCK";

/* 0x020903f8: 16 bytes. */
char data_020903f8[16] = "MERRYGOROUND";

/* 0x02090408: 16 bytes. */
char data_02090408[16] = "CLOCK_HURIKO";

/* 0x02090418: 16 bytes. */
char data_02090418[16] = "TREASURE_BOX";

/* 0x02090428: 16 bytes. */
char data_02090428[16] = "ENTRY_OBJECT";

/* 0x02090438: 16 bytes. */
char data_02090438[16] = "ICE_BLOCK_LL";

/* 0x02090458: 16 bytes. */
char data_02090458[16] = "BOSS_DONKETU";

/* 0x02090478: 16 bytes. */
char data_02090478[16] = "KING_DONKETU";

/* 0x02090498: 16 bytes. */
char data_02090498[16] = "CV_NEWS_LIFT";

/* 0x020903bc: 12 bytes. */
char data_020903bc[12] = "SECRET_COIN";

/* 0x02090350: 12 bytes. */
char data_02090350[12] = "CT_MECHA08A";

/* 0x02090398: 12 bytes. */
char data_02090398[12] = "RC_GURUGURU";

/* 0x0209035c: 12 bytes. */
char data_0209035c[12] = "CT_MECHA08B";

/* 0x02090368: 12 bytes. */
char data_02090368[12] = "CT_MECHA12L";

/* 0x02090320: 12 bytes. */
char data_02090320[12] = "CT_MECHA04L";

/* 0x02090380: 12 bytes. */
char data_02090380[12] = "MG_SNOWBALL";

/* 0x0209038c: 12 bytes. */
char data_0209038c[12] = "KILLER_BOOK";

/* 0x020903b0: 12 bytes. */
char data_020903b0[12] = "EYEKUN_BEAM";

/* 0x02090344: 12 bytes. */
char data_02090344[12] = "FPAKUN_BALL";

/* 0x02090314: 12 bytes. */
char data_02090314[12] = "SL_ICEBLOCK";

/* 0x0209032c: 12 bytes. */
char data_0209032c[12] = "FIREPAKUN_S";

/* 0x020902fc: 12 bytes. */
char data_020902fc[12] = "STAR_CREATE";

/* 0x02090338: 12 bytes. */
char data_02090338[12] = "CT_MECHA04S";

/* 0x020903a4: 12 bytes. */
char data_020903a4[12] = "EYEKUN_BOSS";

/* 0x02090374: 12 bytes. */
char data_02090374[12] = "CT_MECHA12S";

/* 0x02090308: 12 bytes. */
char data_02090308[12] = "WL_POLELIFT";

/* 0x02090528: 16 bytes. */
char data_02090528[16] = "KM1_UKISHIMA";
