/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x02090824..0x02090864, 64 bytes, 3 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x02090838: 20 bytes. */
char data_02090838[20] = "OBJ_VOLCANO_CANNON";

/* 0x02090824: 20 bytes. */
char data_02090824[20] = "KINOKO_CREATE_TAG";

/* 0x0209084c: 24 bytes. */
char data_0209084c[24] = "PROPELLER_HEYHO_FIRE";
