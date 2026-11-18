/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x02090000..0x02090038, 56 bytes, 5 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x02090020: 12 bytes. */
char data_02090020[12] = "BK_ROTEBAR";

/* 0x02090014: 12 bytes. */
char data_02090014[12] = "BK_UKISIMA";

/* 0x02090008: 12 bytes. */
char data_02090008[12] = "BK_BOTAOSI";

/* 0x02090000: 8 bytes. */
char data_02090000[8] = "CHDOOR";

/* 0x0209002c: 12 bytes. */
char data_0209002c[12] = "KI_FUNE_UP";
