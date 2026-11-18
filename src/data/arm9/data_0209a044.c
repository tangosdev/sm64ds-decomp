/* fBase vtable, display and IRQ state.
 * The _ZTV7fBase_c slot array, sIsDispOn, GXi_DmaId and OS_IRQTable.
 * Consumer: fBase_c, GX, OS interrupt dispatch.
 * ROM 0x0209a044..0x0209a050, 12 bytes, 3 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern u8 fsi_def_digest_key[];

/* 0x0209a048: 4 bytes. */
char data_0209a048[4] = "rom";

/* 0x0209a044: 4 bytes. */
char data_0209a044[4] = ":/";

/* 0x0209a04c: 4 bytes. */
const void *fsi_digest_key_ptr = (const void *)&fsi_def_digest_key;
