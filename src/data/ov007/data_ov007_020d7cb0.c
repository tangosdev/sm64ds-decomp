/* 0x020d7cb0: menu layout record (0x1000, 0x9c4, 0x3c, 0, 0x14000, 0).
 * Ov007MeshParams scalar, exact cartridge words. */
#include "types.h"
#include "Ov007Mesh.h"

#pragma define_section sec_020d7cb0 ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7cb0 begin
Ov007MeshParams data_ov007_020d7cb0 = {
    .pointMass = 0x00001000,
    .stiffness = 0x000009c4,
    .damping = 0x0000003c,
    .pushMin = 0x00000000,
    .pushMax = 0x00014000,
    .restZ = 0x00000000,
};
#pragma section sec_020d7cb0 end
