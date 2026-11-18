/* ov034 cartridge data at 0x02113838, 20 bytes. */
#include "types.h"

extern u32 gHanachanBody01ModelFile[];
extern u32 gHanachanBody02ModelFile[];
extern u32 gHanachanBody03ModelFile[];
extern u32 gHanachanBody04ModelFile[];
extern u32 gHanachanHeadModelFile[];

#pragma define_section data_ov034_02113838 ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov034_02113838 begin
void *data_ov034_02113838[5] = {
    (void *)&gHanachanHeadModelFile,   (void *)&gHanachanBody01ModelFile,
    (void *)&gHanachanBody02ModelFile, (void *)&gHanachanBody03ModelFile,
    (void *)&gHanachanBody04ModelFile,
};
#pragma section data_ov034_02113838 end
