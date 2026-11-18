/* Actor spawn table (second half) and the actor debug names.
 * data_02092208 is a 52-word table of overlay profile pointers with no
 * configured symbol names, kept as literals; 0x020922d8..0x02092654 are the
 * ACTOR_DEBUG_NAMES strings the actor dump prints.
 * ROM 0x020922e0..0x020922f8, 24 bytes, 3 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x020922e8: 8 bytes. */
char data_020922e8[8] = "Fire Mt";

/* 0x020922e0: 8 bytes. */
char data_020922e0[8] = "Suisou";

/* 0x020922f0: 8 bytes. */
char data_020922f0[8] = "Snow Mt";
