/* Actor spawn table (second half) and the actor debug names.
 * data_02092208 is a 52-word table of overlay profile pointers with no
 * configured symbol names, kept as literals; 0x020922d8..0x02092654 are the
 * ACTOR_DEBUG_NAMES strings the actor dump prints.
 * ROM 0x020922d8..0x020922e0, 8 bytes, 1 configured symbols.
 */
#include "types.h"

/* 0x020922d8: 8 bytes. */
char data_020922d8[8] = "Cave";
