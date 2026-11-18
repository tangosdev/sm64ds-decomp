/* 0x020d7c9c: menu rectangle record (0x1f4, 0x4fffb, 0xffffff58, ...).
 * Ov007MeshWaveParams scalar, exact cartridge words. */
#include "types.h"
#include "Ov007Mesh.h"

#pragma define_section sec_020d7c9c ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7c9c begin
Ov007MeshWaveParams data_ov007_020d7c9c = {
    .speed = 0x000001f4,
    .spread = 0x0004fffb,
    .amplitude = 0xffffff58,
    .minIdle = 0x0258,
    .keepChance = 0x0320,
    .idleChance = 0x0064,
};
#pragma section sec_020d7c9c end
