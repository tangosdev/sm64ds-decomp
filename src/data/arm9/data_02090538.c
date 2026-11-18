/* Per-language actor debug-name strings.
 * Every record is one language's name string; data_02091000 is the 296-entry
 * pointer table Message and the actor debug dump index them by language.
 * Consumers: data_02091000, ACTOR_DEBUG_NAMES, Message2/Message3.
 * ROM 0x02090538..0x020907c0, 648 bytes, 40 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x02090798: 20 bytes. */
char data_02090798[20] = "BIG_SNOWMAN_HEAD";

/* 0x02090788: 16 bytes. */
char data_02090788[16] = "BLK_SLVSTAR_TAG";

/* 0x02090778: 16 bytes. */
char data_02090778[16] = "BLK_GNSHELL_TAG";

/* 0x02090768: 16 bytes. */
char data_02090768[16] = "BLK_SKINOKO_TAG";

/* 0x02090758: 16 bytes. */
char data_02090758[16] = "BLK_OKINOKO_TAG";

/* 0x02090748: 16 bytes. */
char data_02090748[16] = "PROPELLER_HEYHO";

/* 0x02090738: 16 bytes. */
char data_02090738[16] = "KURIKING_VANISH";

/* 0x02090728: 16 bytes. */
char data_02090728[16] = "MG_TRAMPOLINE2";

/* 0x02090718: 16 bytes. */
char data_02090718[16] = "KM1_KURUMAJIKU";

/* 0x02090708: 16 bytes. */
char data_02090708[16] = "BOOK_GENERATOR";

/* 0x020906f8: 16 bytes. */
char data_020906f8[16] = "PENGUIN_MOTHER";

/* 0x020906e8: 16 bytes. */
char data_020906e8[16] = "WATER_TATUMAKI";

/* 0x020906d8: 16 bytes. */
char data_020906d8[16] = "WATER_HAKIDASI";

/* 0x020906c8: 16 bytes. */
char data_020906c8[16] = "SLIDER_MANAGER";

/* 0x020906b8: 16 bytes. */
char data_020906b8[16] = "BK_DOSSUNBAR_S";

/* 0x020906a8: 16 bytes. */
char data_020906a8[16] = "BK_DOSSUNBAR_L";

/* 0x02090698: 16 bytes. */
char data_02090698[16] = "WANWAN_SHUTTER";

/* 0x02090688: 16 bytes. */
char data_02090688[16] = "HS_UPDOWN_LIFT";

/* 0x02090678: 16 bytes. */
char data_02090678[16] = "CANNON_SHUTTER";

/* 0x02090668: 16 bytes. */
char data_02090668[16] = "PEACH_PRINCESS";

/* 0x02090538: 16 bytes. */
char data_02090538[16] = "OBJ_MARIO_CAP";

/* 0x02090648: 16 bytes. */
char data_02090648[16] = "SNOWMAN_BREATH";

/* 0x02090638: 16 bytes. */
char data_02090638[16] = "KM3_KURUMAJIKU";

/* 0x02090628: 16 bytes. */
char data_02090628[16] = "KM3_YOKOSEESAW";

/* 0x02090618: 16 bytes. */
char data_02090618[16] = "KM2_YOKOSEESAW";

/* 0x02090608: 16 bytes. */
char data_02090608[16] = "PENGUIN_RACER";

/* 0x020905f8: 16 bytes. */
char data_020905f8[16] = "WATER_SUIKOMI";

/* 0x020905e8: 16 bytes. */
char data_020905e8[16] = "MG_TRAMPOLINE";

/* 0x020905d8: 16 bytes. */
char data_020905d8[16] = "OBJ_BLUE_FIRE";

/* 0x020905c8: 16 bytes. */
char data_020905c8[16] = "KI_HASIRA_DAI";

/* 0x02090658: 16 bytes. */
char data_02090658[16] = "SCALEUP_KINOKO";

/* 0x020905a8: 16 bytes. */
char data_020905a8[16] = "BK_KILLER_DAI";

/* 0x02090598: 16 bytes. */
char data_02090598[16] = "UDLIFT_TERESA";

/* 0x02090588: 16 bytes. */
char data_02090588[16] = "HATENA_SWITCH";

/* 0x02090578: 16 bytes. */
char data_02090578[16] = "KB1_BILLBOARD";

/* 0x020905b8: 16 bytes. */
char data_020905b8[16] = "POWER_UP_ITEM";

/* 0x02090558: 16 bytes. */
char data_02090558[16] = "EWM_ICE_BLOCK";

/* 0x02090548: 16 bytes. */
char data_02090548[16] = "KM3_KAITENDAI";

/* 0x02090568: 16 bytes. */
char data_02090568[16] = "VS_ITEM_BLOCK";

/* 0x020907ac: 20 bytes. */
char data_020907ac[20] = "BIG_SNOWMAN_BODY";
