/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x02090038..0x02090128, 240 bytes, 20 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x02090110: 12 bytes. */
char data_02090110[12] = "CT_MECHA09";

/* 0x02090104: 12 bytes. */
char data_02090104[12] = "CT_MECHA07";

/* 0x020900f8: 12 bytes. */
char data_020900f8[12] = "CT_MECHA06";

/* 0x020900ec: 12 bytes. */
char data_020900ec[12] = "CT_MECHA05";

/* 0x020900e0: 12 bytes. */
char data_020900e0[12] = "CT_MECHA03";

/* 0x020900d4: 12 bytes. */
char data_020900d4[12] = "CT_MECHA02";

/* 0x020900c8: 12 bytes. */
char data_020900c8[12] = "CT_MECHA01";

/* 0x020900bc: 12 bytes. */
char data_020900bc[12] = "SW_BLOCK_L";

/* 0x020900b0: 12 bytes. */
char data_020900b0[12] = "KINOKO_TAG";

/* 0x020900a4: 12 bytes. */
char data_020900a4[12] = "WATER_RING";

/* 0x02090098: 12 bytes. */
char data_02090098[12] = "WC_UKISIMA";

/* 0x0209008c: 12 bytes. */
char data_0209008c[12] = "MG_CURLING";

/* 0x02090080: 12 bytes. */
char data_02090080[12] = "SANBO_BODY";

/* 0x02090074: 12 bytes. */
char data_02090074[12] = "DL_PYRAMID";

/* 0x02090068: 12 bytes. */
char data_02090068[12] = "FL_UKIYUKA";

/* 0x0209005c: 12 bytes. */
char data_0209005c[12] = "FL_AMILIFT";

/* 0x02090050: 12 bytes. */
char data_02090050[12] = "CV_SHUTTER";

/* 0x02090044: 12 bytes. */
char data_02090044[12] = "MG_BS_CARD";

/* 0x02090038: 12 bytes. */
char data_02090038[12] = "CHORO_ROCK";

/* 0x0209011c: 12 bytes. */
char data_0209011c[12] = "CT_MECHA10";
