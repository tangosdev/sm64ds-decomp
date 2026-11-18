/* fBase vtable, display and IRQ state.
 * The _ZTV7fBase_c slot array, sIsDispOn, GXi_DmaId and OS_IRQTable.
 * Consumer: fBase_c, GX, OS interrupt dispatch.
 * ROM 0x0209a068..0x0209a078, 16 bytes, 4 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0209a070: 4 bytes. */
int data_0209a070 = 0x00000001;

/* 0x0209a06c: 4 bytes. */
int data_0209a06c = (int)0xffffffffu;

/* 0x0209a068: 4 bytes. */
int data_0209a068 = (int)0xffffffffu;

/* 0x0209a074: 4 bytes. */
int data_0209a074 = (int)0xffffffffu;
