/* fBase vtable, display and IRQ state.
 * The _ZTV7fBase_c slot array, sIsDispOn, GXi_DmaId and OS_IRQTable.
 * Consumer: fBase_c, GX, OS interrupt dispatch.
 * ROM 0x02099f88..0x02099fa4, 28 bytes, 2 configured symbols.
 */
#include "types.h"

typedef struct {
    int x, y, z;
} VecFx32;

/* 0x02099f88: 12 bytes. */
VecFx32 data_02099f88 = {0x00001000, 0x00001000, 0x00001000};

/* 0x02099f94: 16 bytes. */
u8 data_02099f94[16] = {
    0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x00, 0x80, 0x00, 0x00, 0x01, 0x00, 0x02,
};
