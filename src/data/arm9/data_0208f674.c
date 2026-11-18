/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x0208f674..0x0208f690, 28 bytes, 5 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208f680: 8 bytes. */
char data_0208f680[8] = "STAR";

/* 0x0208f67c: 4 bytes. */
char data_0208f67c[4] = "MAP";

/* 0x0208f678: 4 bytes. */
char data_0208f678[4] = "MIP";

/* 0x0208f674: 4 bytes. */
char data_0208f674[4] = "BAR";

/* 0x0208f688: 8 bytes. */
char data_0208f688[8] = "DEMO";
