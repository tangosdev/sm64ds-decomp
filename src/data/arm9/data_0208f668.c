/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x0208f668..0x0208f670, 8 bytes, 1 configured symbols.
 */
#include "types.h"

/* 0x0208f668: 8 bytes. */
unsigned char data_0208f668[8] = {
    0x4c, 0x5a, 0x37, 0x37, 0x00, 0x00, 0x00, 0x00,
};
