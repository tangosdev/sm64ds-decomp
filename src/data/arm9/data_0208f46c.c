/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x0208f46c..0x0208f5e0, 372 bytes, 10 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208f540: 40 bytes. */
char _ZTSN5dPa_c7level_c18fitWaterCallback_cE[40] = "N5dPa_c7level_c18fitWaterCallback_cE";

/* 0x0208f568: 40 bytes. */
char _ZTSN5dPa_c7level_c20checkWaterCallback_cE[40] = "N5dPa_c7level_c20checkWaterCallback_cE";

/* 0x0208f590: 40 bytes. */
char _ZTSN5dPa_c7level_c20edStarKiraCallback_cE[40] = "N5dPa_c7level_c20edStarKiraCallback_cE";

/* 0x0208f51c: 36 bytes. */
char _ZTSN5dPa_c7level_c16simpleCallback_cE[36] = "N5dPa_c7level_c16simpleCallback_cE";

/* 0x0208f4f8: 36 bytes. */
char _ZTSN5dPa_c7level_c16splashCallback_cE[36] = "N5dPa_c7level_c16splashCallback_cE";

/* 0x0208f4b0: 36 bytes. */
char _ZTSN5dPa_c7level_c15scaleCallback_cE[36] = "N5dPa_c7level_c15scaleCallback_cE";

/* 0x0208f4d4: 36 bytes. */
char _ZTSN5dPa_c7level_c16bubbleCallback_cE[36] = "N5dPa_c7level_c16bubbleCallback_cE";

/* 0x0208f48c: 36 bytes. */
char _ZTSN5dPa_c7level_c14clipCallback_cE[36] = "N5dPa_c7level_c14clipCallback_cE";

/* 0x0208f46c: 32 bytes. */
char _ZTSN5dPa_c7level_c10callback_cE[32] = "N5dPa_c7level_c10callback_cE";

/* 0x0208f5b8: 40 bytes. */
char _ZTSN5dPa_c7level_c20checkYoganCallback_cE[40] = "N5dPa_c7level_c20checkYoganCallback_cE";
