/* ov034 cartridge data at 0x0211384c, 20 bytes. */
#include "types.h"

extern u32 gHanachanBody01TextureSequenceFile[];
extern u32 gHanachanBody02TextureSequenceFile[];
extern u32 gHanachanBody03TextureSequenceFile[];
extern u32 gHanachanBody04TextureSequenceFile[];
extern u32 gHanachanHeadTextureSequenceFile[];

#pragma define_section data_ov034_0211384c ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov034_0211384c begin
void *data_ov034_0211384c[5] = {
    (void *)&gHanachanHeadTextureSequenceFile,   (void *)&gHanachanBody01TextureSequenceFile,
    (void *)&gHanachanBody02TextureSequenceFile, (void *)&gHanachanBody03TextureSequenceFile,
    (void *)&gHanachanBody04TextureSequenceFile,
};
#pragma section data_ov034_0211384c end
