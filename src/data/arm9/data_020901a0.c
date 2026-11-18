/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x020901a0..0x020901dc, 60 bytes, 5 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x020901c4: 12 bytes. */
char data_020901c4[12] = "BIG_SNOWMAN";

/* 0x020901b8: 12 bytes. */
char data_020901b8[12] = "KM2_RIFUT01";

/* 0x020901ac: 12 bytes. */
char data_020901ac[12] = "KM2_RIFUT02";

/* 0x020901a0: 12 bytes. */
char data_020901a0[12] = "OBJ_1UPLOGO";

/* 0x020901d0: 12 bytes. */
char data_020901d0[12] = "MG_MEMORY_J";
