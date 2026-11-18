/* ov034 cartridge data at 0x0211384c, 20 bytes. */
#include "types.h"

extern u32 gHanachanBody01TextureSequenceFile[];
extern u32 gHanachanBody02TextureSequenceFile[];
extern u32 gHanachanBody03TextureSequenceFile[];
extern u32 gHanachanBody04TextureSequenceFile[];
extern u32 gHanachanHeadTextureSequenceFile[];

const u32 data_ov034_0211384c[5] = {
    (u32)&gHanachanHeadTextureSequenceFile, (u32)&gHanachanBody01TextureSequenceFile, (u32)&gHanachanBody02TextureSequenceFile, (u32)&gHanachanBody03TextureSequenceFile, (u32)&gHanachanBody04TextureSequenceFile,
};
