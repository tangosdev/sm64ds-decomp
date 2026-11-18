/* ov065 data_ov065_0211cfd8 at 0x0211cfd8 (data, 16 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov065_0211cfd8[4] = {
    0x02112138,  /* overlays(12,13,24,33,35,43,45,47,52,53,56):0x02112138 */
    0x0211d824,  /* gCtMechaObj02ModelFile */
    0x0211d82c,  /* gCtMechaObj02CollisionFile */
    0x02112178,  /* overlays(12,13,24,33,35,43,45,47,52,53,56):0x02112178 */
};

#pragma section dc8_section end
