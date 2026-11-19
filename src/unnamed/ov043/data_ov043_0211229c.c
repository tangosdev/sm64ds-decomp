/* Cartridge .data hosted from source. Placeholder name kept.
 * Pointer words are the relocs; the other words are the table.
 */
#include "types.h"

extern u32 data_ov009_02112270[];

u32 data_ov043_0211229c[7] = {
    0x00000002,
    0x00000000,
    0x00300000,
    0x01000000,
    0x00000000,
    0x00000000,
    (u32)&data_ov009_02112270,
};
