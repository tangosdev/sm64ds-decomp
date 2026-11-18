/* ov034 cartridge data at 0x02113888, 20 bytes. */
#include "types.h"

extern u32 gHanachanBody01WalkStartAnimationFile[];
extern u32 gHanachanBody02WalkStartAnimationFile[];
extern u32 gHanachanBody03WalkStartAnimationFile[];
extern u32 gHanachanBody04WalkStartAnimationFile[];
extern u32 gHanachanHeadWalkStartAnimationFile[];

#pragma define_section data_ov034_02113888 ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov034_02113888 begin
void *data_ov034_02113888[5] = {
    (void *)&gHanachanHeadWalkStartAnimationFile,   (void *)&gHanachanBody01WalkStartAnimationFile,
    (void *)&gHanachanBody02WalkStartAnimationFile, (void *)&gHanachanBody03WalkStartAnimationFile,
    (void *)&gHanachanBody04WalkStartAnimationFile,
};
#pragma section data_ov034_02113888 end
