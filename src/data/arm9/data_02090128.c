/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x02090128..0x020901a0, 120 bytes, 10 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x02090188: 12 bytes. */
char data_02090188[12] = "KM2_AMI_BOU";

/* 0x0209017c: 12 bytes. */
char data_0209017c[12] = "ITEM_BLOCK";

/* 0x02090170: 12 bytes. */
char data_02090170[12] = "KM1_KURUMA";

/* 0x02090164: 12 bytes. */
char data_02090164[12] = "KM1_DORIFU";

/* 0x02090158: 12 bytes. */
char data_02090158[12] = "KM1_SEESAW";

/* 0x0209014c: 12 bytes. */
char data_0209014c[12] = "NOKONOKO_S";

/* 0x02090140: 12 bytes. */
char data_02090140[12] = "RC_BURANKO";

/* 0x02090134: 12 bytes. */
char data_02090134[12] = "SHOOT_BOOK";

/* 0x02090128: 12 bytes. */
char data_02090128[12] = "CT_MECHA11";

/* 0x02090194: 12 bytes. */
char data_02090194[12] = "MONKEY_STAR";
