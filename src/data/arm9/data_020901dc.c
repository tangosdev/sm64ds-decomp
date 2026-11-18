/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x020901dc..0x020901f4, 24 bytes, 2 configured symbols.
 */
#include "types.h"

/* 0x020901dc: 12 bytes. */
char data_020901dc[12] = "MG_HANACHAN";

/* 0x020901e8: 12 bytes. */
char data_020901e8[12] = "ONEUPKINOKO";
