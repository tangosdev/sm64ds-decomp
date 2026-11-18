/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x0208f690..0x0208f730, 160 bytes, 20 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208f720: 8 bytes. */
char data_0208f720[8] = "KIRAI";

/* 0x0208f718: 8 bytes. */
char data_0208f718[8] = "TITLE";

/* 0x0208f710: 8 bytes. */
char data_0208f710[8] = "METER";

/* 0x0208f708: 8 bytes. */
char data_0208f708[8] = "STAGE";

/* 0x0208f700: 8 bytes. */
char data_0208f700[8] = "SBIRD";

/* 0x0208f6f8: 8 bytes. */
char data_0208f6f8[8] = "PIANO";

/* 0x0208f6f0: 8 bytes. */
char data_0208f6f0[8] = "PAKUN";

/* 0x0208f6e8: 8 bytes. */
char data_0208f6e8[8] = "CHAIR";

/* 0x0208f6e0: 8 bytes. */
char data_0208f6e0[8] = "POPOI";

/* 0x0208f6d8: 8 bytes. */
char data_0208f6d8[8] = "DOKAN";

/* 0x0208f6d0: 8 bytes. */
char data_0208f6d0[8] = "JANGO";

/* 0x0208f6c8: 8 bytes. */
char data_0208f6c8[8] = "COIN";

/* 0x0208f6c0: 8 bytes. */
char data_0208f6c0[8] = "TREE";

/* 0x0208f6b8: 8 bytes. */
char data_0208f6b8[8] = "PILE";

/* 0x0208f6b0: 8 bytes. */
char data_0208f6b0[8] = "LAVA";

/* 0x0208f6a8: 8 bytes. */
char data_0208f6a8[8] = "DSMT";

/* 0x0208f6a0: 8 bytes. */
char data_0208f6a0[8] = "FISH";

/* 0x0208f698: 8 bytes. */
char data_0208f698[8] = "BOOT";

/* 0x0208f690: 8 bytes. */
char data_0208f690[8] = "DOOR";

/* 0x0208f728: 8 bytes. */
char data_0208f728[8] = "LUIGI";
