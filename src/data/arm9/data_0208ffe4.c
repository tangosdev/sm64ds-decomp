/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x0208ffe4..0x0208fff0, 12 bytes, 1 configured symbols.
 */
#include "types.h"

/* 0x0208ffe4: 12 bytes. */
char data_0208ffe4[12] = "YAJIRUSI_R";
