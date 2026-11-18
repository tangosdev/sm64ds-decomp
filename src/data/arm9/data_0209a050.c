/* fBase vtable, display and IRQ state.
 * The _ZTV7fBase_c slot array, sIsDispOn, GXi_DmaId and OS_IRQTable.
 * Consumer: fBase_c, GX, OS interrupt dispatch.
 * ROM 0x0209a050..0x0209a054, 4 bytes, 1 configured symbols.
 */
#include "types.h"

/* 0x0209a050: 4 bytes. */
int fsi_digest_key_len = 0x00000040;
