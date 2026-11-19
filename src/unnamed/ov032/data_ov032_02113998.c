/* Cartridge .data hosted from source. Placeholder name kept.
 * Pointer words are the relocs; the other words are the table.
 */
#include "types.h"

extern u32 data_ov021_02113960[];

u32 data_ov032_02113998[3] = {
    0x00050000,
    0x00000000,
    (u32)&data_ov021_02113960,
};
