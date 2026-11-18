/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x0209020c..0x020902fc, 240 bytes, 20 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x020902e4: 12 bytes. */
char data_020902e4[12] = "MG_ROULETTE";

/* 0x020902d8: 12 bytes. */
char data_020902d8[12] = "OBJ_MIP_KEY";

/* 0x020902cc: 12 bytes. */
char data_020902cc[12] = "BK_TRANSBAR";

/* 0x020902c0: 12 bytes. */
char data_020902c0[12] = "ICE_DONKETU";

/* 0x020902b4: 12 bytes. */
char data_020902b4[12] = "BOMB_SEESAW";

/* 0x020902a8: 12 bytes. */
char data_020902a8[12] = "BOOK_SWITCH";

/* 0x0209029c: 12 bytes. */
char data_0209029c[12] = "ICON_TERESA";

/* 0x02090290: 12 bytes. */
char data_02090290[12] = "UPDOWN_LIFT";

/* 0x02090284: 12 bytes. */
char data_02090284[12] = "BOSS_TERESA";

/* 0x02090278: 12 bytes. */
char data_02090278[12] = "CAP_BLOCK_W";

/* 0x0209026c: 12 bytes. */
char data_0209026c[12] = "CAP_BLOCK_L";

/* 0x02090260: 12 bytes. */
char data_02090260[12] = "CAP_BLOCK_M";

/* 0x02090254: 12 bytes. */
char data_02090254[12] = "STAR_SWITCH";

/* 0x02090248: 12 bytes. */
char data_02090248[12] = "STAR_SELECT";

/* 0x0209023c: 12 bytes. */
char data_0209023c[12] = "BATTA_BLOCK";

/* 0x02090230: 12 bytes. */
char data_02090230[12] = "CLOCK_SHORT";

/* 0x02090224: 12 bytes. */
char data_02090224[12] = "MG_PACHINKO";

/* 0x02090218: 12 bytes. */
char data_02090218[12] = "RED_BOMBHEI";

/* 0x0209020c: 12 bytes. */
char data_0209020c[12] = "SILVER_STAR";

/* 0x020902f0: 12 bytes. */
char data_020902f0[12] = "YUREI_MUCHO";
