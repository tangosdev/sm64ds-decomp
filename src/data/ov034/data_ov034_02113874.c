/* ov034 cartridge data at 0x02113874, 20 bytes. */
#include "types.h"

extern u32 gHanachanBody01WalkAnimationFile[];
extern u32 gHanachanBody02WalkAnimationFile[];
extern u32 gHanachanBody03WalkAnimationFile[];
extern u32 gHanachanBody04WalkAnimationFile[];
extern u32 gHanachanHeadWalkAnimationFile[];

#pragma define_section data_ov034_02113874 ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov034_02113874 begin
void *data_ov034_02113874[5] = {
    (void *)&gHanachanHeadWalkAnimationFile,   (void *)&gHanachanBody01WalkAnimationFile,
    (void *)&gHanachanBody02WalkAnimationFile, (void *)&gHanachanBody03WalkAnimationFile,
    (void *)&gHanachanBody04WalkAnimationFile,
};
#pragma section data_ov034_02113874 end
