/* fBase vtable, display and IRQ state.
 * The _ZTV7fBase_c slot array, sIsDispOn, GXi_DmaId and OS_IRQTable.
 * Consumer: fBase_c, GX, OS interrupt dispatch.
 * ROM 0x02099fd0..0x02099fd4, 4 bytes, 1 configured symbols.
 */
#include "types.h"

/* 0x02099fd0: 4 bytes. */
int GXi_DmaId = 0x00000003;
