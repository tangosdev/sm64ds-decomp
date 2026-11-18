/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x0208f730..0x0208f870, 320 bytes, 40 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208f860: 8 bytes. */
char data_0208f860[8] = "KANBAN";

/* 0x0208f858: 8 bytes. */
char data_0208f858[8] = "PLAYER";

/* 0x0208f850: 8 bytes. */
char data_0208f850[8] = "BUBBLE";

/* 0x0208f848: 8 bytes. */
char data_0208f848[8] = "CANNON";

/* 0x0208f840: 8 bytes. */
char data_0208f840[8] = "TOGEZO";

/* 0x0208f838: 8 bytes. */
char data_0208f838[8] = "HOLHEI";

/* 0x0208f830: 8 bytes. */
char data_0208f830[8] = "MG_CUP";

/* 0x0208f828: 8 bytes. */
char data_0208f828[8] = "RESULT";

/* 0x0208f820: 8 bytes. */
char data_0208f820[8] = "HYUHYU";

/* 0x0208f818: 8 bytes. */
char data_0208f818[8] = "EYEKUN";

/* 0x0208f810: 8 bytes. */
char data_0208f810[8] = "WANWAN";

/* 0x0208f808: 8 bytes. */
char data_0208f808[8] = "KAIDAN";

/* 0x0208f800: 8 bytes. */
char data_0208f800[8] = "CASKET";

/* 0x0208f7f8: 8 bytes. */
char data_0208f7f8[8] = "KILLER";

/* 0x0208f7f0: 8 bytes. */
char data_0208f7f0[8] = "END_OF";

/* 0x0208f7e8: 8 bytes. */
char data_0208f7e8[8] = "KI_ITA";

/* 0x0208f7e0: 8 bytes. */
char data_0208f7e0[8] = "CAMERA";

/* 0x0208f7d8: 8 bytes. */
char data_0208f7d8[8] = "KURIBO";

/* 0x0208f7d0: 8 bytes. */
char data_0208f7d0[8] = "SET_SE";

/* 0x0208f7c8: 8 bytes. */
char data_0208f7c8[8] = "UDLIFT";

/* 0x0208f7c0: 8 bytes. */
char data_0208f7c0[8] = "WAKAME";

/* 0x0208f7b8: 8 bytes. */
char data_0208f7b8[8] = "PAKUN2";

/* 0x0208f7b0: 8 bytes. */
char data_0208f7b0[8] = "SPIDER";

/* 0x0208f7a8: 8 bytes. */
char data_0208f7a8[8] = "TERESA";

/* 0x0208f7a0: 8 bytes. */
char data_0208f7a0[8] = "KI_IWA";

/* 0x0208f798: 8 bytes. */
char data_0208f798[8] = "IWANTE";

/* 0x0208f790: 8 bytes. */
char data_0208f790[8] = "BATAN";

/* 0x0208f788: 8 bytes. */
char data_0208f788[8] = "SHARK";

/* 0x0208f780: 8 bytes. */
char data_0208f780[8] = "MORAY";

/* 0x0208f778: 8 bytes. */
char data_0208f778[8] = "JUGEM";

/* 0x0208f770: 8 bytes. */
char data_0208f770[8] = "SHELL";

/* 0x0208f768: 8 bytes. */
char data_0208f768[8] = "ENTRY";

/* 0x0208f760: 8 bytes. */
char data_0208f760[8] = "SANBO";

/* 0x0208f758: 8 bytes. */
char data_0208f758[8] = "KOOPA";

/* 0x0208f750: 8 bytes. */
char data_0208f750[8] = "DOSSY";

/* 0x0208f748: 8 bytes. */
char data_0208f748[8] = "HEART";

/* 0x0208f740: 8 bytes. */
char data_0208f740[8] = "MANTA";

/* 0x0208f738: 8 bytes. */
char data_0208f738[8] = "DOSUN";

/* 0x0208f730: 8 bytes. */
char data_0208f730[8] = "MENBO";

/* 0x0208f868: 8 bytes. */
char data_0208f868[8] = "SEESAW";
