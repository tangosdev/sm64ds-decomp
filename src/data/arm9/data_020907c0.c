/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x020907c0..0x020907fc, 60 bytes, 3 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x020907d4: 20 bytes. */
char data_020907d4[20] = "WL_KOOPA_SHUTTER";

/* 0x020907c0: 20 bytes. */
char data_020907c0[20] = "DL_PYRAMID_DUMMY";

/* 0x020907e8: 20 bytes. */
char data_020907e8[20] = "SL_ICEBLOCK_SHOT";
