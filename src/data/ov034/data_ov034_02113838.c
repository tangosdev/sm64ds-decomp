/* ov034 cartridge data at 0x02113838, 20 bytes. */
#include "types.h"

extern u32 gHanachanBody01ModelFile[];
extern u32 gHanachanBody02ModelFile[];
extern u32 gHanachanBody03ModelFile[];
extern u32 gHanachanBody04ModelFile[];
extern u32 gHanachanHeadModelFile[];

const u32 data_ov034_02113838[5] = {
    (u32)&gHanachanHeadModelFile, (u32)&gHanachanBody01ModelFile, (u32)&gHanachanBody02ModelFile, (u32)&gHanachanBody03ModelFile, (u32)&gHanachanBody04ModelFile,
};
