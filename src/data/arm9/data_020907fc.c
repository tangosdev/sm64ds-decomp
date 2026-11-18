/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x020907fc..0x02090824, 40 bytes, 3 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x02090800: 16 bytes. */
char data_02090800[16] = "FLAMETHROWER";

/* 0x020907fc: 4 bytes. */
char data_020907fc[4] = "OBJ_";

/* 0x02090810: 20 bytes. */
char data_02090810[20] = "PENGUIN_DEFENDER";
