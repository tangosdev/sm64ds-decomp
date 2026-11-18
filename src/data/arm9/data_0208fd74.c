/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x0208fd74..0x0208ff54, 480 bytes, 40 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208ff3c: 12 bytes. */
char data_0208ff3c[12] = "OBJ_NUMBER";

/* 0x0208ff30: 12 bytes. */
char data_0208ff30[12] = "KM2_KUZURE";

/* 0x0208ff24: 12 bytes. */
char data_0208ff24[12] = "FL_UKI_KI";

/* 0x0208ff18: 12 bytes. */
char data_0208ff18[12] = "EWB_ICE_A";

/* 0x0208ff0c: 12 bytes. */
char data_0208ff0c[12] = "BC_SWITCH";

/* 0x0208ff00: 12 bytes. */
char data_0208ff00[12] = "RC_RIFT02";

/* 0x0208fef4: 12 bytes. */
char data_0208fef4[12] = "RC_CARPET";

/* 0x0208fee8: 12 bytes. */
char data_0208fee8[12] = "GAMAGUCHI";

/* 0x0208fedc: 12 bytes. */
char data_0208fedc[12] = "RC_KAITEN";

/* 0x0208fed0: 12 bytes. */
char data_0208fed0[12] = "RC_SEESAW";

/* 0x0208fec4: 12 bytes. */
char data_0208fec4[12] = "RC_TIKUWA";

/* 0x0208feb8: 12 bytes. */
char data_0208feb8[12] = "MULTIBOOT";

/* 0x0208feac: 12 bytes. */
char data_0208feac[12] = "RC_RIFT01";

/* 0x0208fea0: 12 bytes. */
char data_0208fea0[12] = "RC_DORIFU";

/* 0x0208fe94: 12 bytes. */
char data_0208fe94[12] = "RACE_FLAG";

/* 0x0208fe88: 12 bytes. */
char data_0208fe88[12] = "SOUND_OBJ";

/* 0x0208fe7c: 12 bytes. */
char data_0208fe7c[12] = "FL_PUZZLE";

/* 0x0208fe70: 12 bytes. */
char data_0208fe70[12] = "HM_BASKET";

/* 0x0208fe64: 12 bytes. */
char data_0208fe64[12] = "HM_MARUTA";

/* 0x0208fe58: 12 bytes. */
char data_0208fe58[12] = "FM_BATTAN";

/* 0x0208fe4c: 12 bytes. */
char data_0208fe4c[12] = "FL_KUZURE";

/* 0x0208fe40: 12 bytes. */
char data_0208fe40[12] = "FL_KOMA_U";

/* 0x0208fe34: 12 bytes. */
char data_0208fe34[12] = "FL_KOMA_D";

/* 0x0208fe28: 12 bytes. */
char data_0208fe28[12] = "OBJ_SHELL";

/* 0x0208fe1c: 12 bytes. */
char data_0208fe1c[12] = "FL_SEESAW";

/* 0x0208fe10: 12 bytes. */
char data_0208fe10[12] = "STAR_GATE";

/* 0x0208fe04: 12 bytes. */
char data_0208fe04[12] = "SLIDE_BOX";

/* 0x0208fdf8: 12 bytes. */
char data_0208fdf8[12] = "FL_LONDON";

/* 0x0208fdec: 12 bytes. */
char data_0208fdec[12] = "FL_MARUTA";

/* 0x0208fde0: 12 bytes. */
char data_0208fde0[12] = "WATERBOMB";

/* 0x0208fdd4: 12 bytes. */
char data_0208fdd4[12] = "TH_DOWN_B";

/* 0x0208fdc8: 12 bytes. */
char data_0208fdc8[12] = "KI_HASIRA";

/* 0x0208fdbc: 12 bytes. */
char data_0208fdbc[12] = "MUGEN_BGM";

/* 0x0208fdb0: 12 bytes. */
char data_0208fdb0[12] = "BK_LIFT01";

/* 0x0208fda4: 12 bytes. */
char data_0208fda4[12] = "GORO_ROCK";

/* 0x0208fd98: 12 bytes. */
char data_0208fd98[12] = "BK_KABE00";

/* 0x0208fd8c: 12 bytes. */
char data_0208fd8c[12] = "BK_KABE01";

/* 0x0208fd80: 12 bytes. */
char data_0208fd80[12] = "BK_DOWN_B";

/* 0x0208fd74: 12 bytes. */
char data_0208fd74[12] = "HS_Y_STAR";

/* 0x0208ff48: 12 bytes. */
char data_0208ff48[12] = "KM2_SUSUMU";
