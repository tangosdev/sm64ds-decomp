/* 0x020d7cc8: menu layout record (3, 0x1000, 0x7d0, 0x7d0, 0x50000, ...).
 * Ov007SwingParams scalar, exact cartridge words. */
#include "types.h"
#include "Ov007Swing.h"

#pragma define_section sec_020d7cc8 ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7cc8 begin
Ov007SwingParams data_ov007_020d7cc8 = {
    .chainLength = 0x00000003,
    .chainPointMass = 0x00001000,
    .chainStiffness = 0x000007d0,
    .chainDamping = 0x000007d0,
    .normalScale = 0x00050000,
    .sidePointMass = 0x00001000,
    .sideStiffness = 0x000003e8,
    .sideDamping = 0x000001f4,
    .pad_20 = { 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 },
    .maxRotX = 0x0888,
    .maxRotY = 0x0aaa,
    .maxNormalX = 0x0100,
    .maxNormalY = 0x0100,
};
#pragma section sec_020d7cc8 end
