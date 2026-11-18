/* fBase vtable, display and IRQ state.
 * The _ZTV7fBase_c slot array, sIsDispOn, GXi_DmaId and OS_IRQTable.
 * Consumer: fBase_c, GX, OS interrupt dispatch.
 * ROM 0x0209a03c..0x0209a044, 8 bytes, 2 configured symbols.
 */
#include "types.h"

extern void func_02057e30();

/* 0x0209a03c: 4 bytes. */
void (*data_0209a03c)(void *) = (void (*)(void *))&func_02057e30;

/* 0x0209a040: 4 bytes. */
int data_0209a040 = (int)0xffffffffu;
