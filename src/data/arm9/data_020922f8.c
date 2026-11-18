/* Actor spawn table (second half) and the actor debug names.
 * data_02092208 is a 52-word table of overlay profile pointers with no
 * configured symbol names, kept as literals; 0x020922d8..0x02092654 are the
 * ACTOR_DEBUG_NAMES strings the actor dump prints.
 * ROM 0x020922f8..0x02092330, 56 bytes, 5 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x02092318: 12 bytes. */
char data_02092318[12] = "Horisoko";

/* 0x0209230c: 12 bytes. */
char data_0209230c[12] = "Test Map";

/* 0x02092300: 12 bytes. */
char data_02092300[12] = "Habatake";

/* 0x020922f8: 8 bytes. */
char data_020922f8[8] = "Vs Main";

/* 0x02092324: 12 bytes. */
char data_02092324[12] = "Vs Cross";
