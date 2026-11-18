/* fBase vtable, display and IRQ state.
 * The _ZTV7fBase_c slot array, sIsDispOn, GXi_DmaId and OS_IRQTable.
 * Consumer: fBase_c, GX, OS interrupt dispatch.
 * ROM 0x0209a054..0x0209a064, 16 bytes, 1 configured symbols.
 */
#include "types.h"

typedef struct {
    u32 w[4];
} T4;

/* 0x0209a054: 16 bytes. */
T4 data_0209a054 = {{0x5a827999u, 0x6ed9eba1u, 0x8f1bbcdcu, 0xca62c1d6u}};
