/* 0x020d7cf8: menu layout record (0, 0, 0, 0xa0000, 0xa0000, ...).
 * Ov007MeshMaterial[2], exact cartridge words. */
#include "types.h"
#include "Ov007Mesh.h"

#pragma define_section sec_020d7cf8 ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7cf8 begin
Ov007MeshMaterial data_ov007_020d7cf8[2] = {
    {
        .texture = 0x00000000,
        .texS = 0x00000000,
        .texT = 0x00000000,
        .warpS = 0x000a0000,
        .warpT = 0x000a0000,
        .tilesS = 0x00001000,
        .tilesT = 0x00001000,
        .texRepeat = 0x00000000,
        .texColor0 = 0x00000000,
        .alpha = 0x0000001f,
        .diffuse = 0x7fff,
        .ambient = 0x0000,
        .polygonMode = 0x00000002,
        .polygonId = 0x00000020,
    },
    {
        .texture = 0x00000000,
        .texS = 0x00000000,
        .texT = 0x00000000,
        .warpS = 0x0012c000,
        .warpT = 0x0012c000,
        .tilesS = 0x00001000,
        .tilesT = 0x00001000,
        .texRepeat = 0x00000000,
        .texColor0 = 0x00000000,
        .alpha = 0x0000000f,
        .diffuse = 0x7fff,
        .ambient = 0x0000,
        .polygonMode = 0x00000000,
        .polygonId = 0x0000001f,
    },
};
#pragma section sec_020d7cf8 end
