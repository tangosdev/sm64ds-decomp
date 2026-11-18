/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x02090200..0x02090208, 8 bytes, 1 configured symbols.
 */
#include "types.h"

/* 0x02090200: 8 bytes. */
char data_02090200[8] = "MC_METAL";
