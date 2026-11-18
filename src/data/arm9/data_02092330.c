/* Actor spawn table (second half) and the actor debug names.
 * data_02092208 is a 52-word table of overlay profile pointers with no
 * configured symbol names, kept as literals; 0x020922d8..0x02092654 are the
 * ACTOR_DEBUG_NAMES strings the actor dump prints.
 * ROM 0x02092330..0x020923a8, 120 bytes, 10 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x02092390: 12 bytes. */
char data_02092390[12] = "Castle 1F";

/* 0x02092384: 12 bytes. */
char data_02092384[12] = "Desert Py";

/* 0x02092378: 12 bytes. */
char data_02092378[12] = "Fire Land";

/* 0x0209236c: 12 bytes. */
char data_0209236c[12] = "Snow Kama";

/* 0x02092360: 12 bytes. */
char data_02092360[12] = "Vs Island";

/* 0x02092354: 12 bytes. */
char data_02092354[12] = "Snow Land";

/* 0x02092348: 12 bytes. */
char data_02092348[12] = "Castle B1";

/* 0x0209233c: 12 bytes. */
char data_0209233c[12] = "Castle 2F";

/* 0x02092330: 12 bytes. */
char data_02092330[12] = "Hight Mt";

/* 0x0209239c: 12 bytes. */
char data_0209239c[12] = "Play Room";
