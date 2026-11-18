/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x0208ff54..0x0208ffcc, 120 bytes, 10 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208ffb4: 12 bytes. */
char data_0208ffb4[12] = "MG_TAMAIRE";

/* 0x0208ffa8: 12 bytes. */
char data_0208ffa8[12] = "CLOCK_LONG";

/* 0x0208ff9c: 12 bytes. */
char data_0208ff9c[12] = "KM3_DERU02";

/* 0x0208ff90: 12 bytes. */
char data_0208ff90[12] = "KM3_DERU01";

/* 0x0208ff84: 12 bytes. */
char data_0208ff84[12] = "KM3_DORIFU";

/* 0x0208ff78: 12 bytes. */
char data_0208ff78[12] = "CAMERA_TAG";

/* 0x0208ff6c: 12 bytes. */
char data_0208ff6c[12] = "KM3_KURUMA";

/* 0x0208ff60: 12 bytes. */
char data_0208ff60[12] = "KM3_SEESAW";

/* 0x0208ff54: 12 bytes. */
char data_0208ff54[12] = "KM2_NOBIRU";

/* 0x0208ffc0: 12 bytes. */
char data_0208ffc0[12] = "YAJIRUSI_L";
