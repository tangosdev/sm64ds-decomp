/* ov034 cartridge data at 0x0211389c, 12 bytes. */
#include "types.h"

extern u32 gHanachanBody01WalkEndAnimationFile[];
extern u32 gHanachanBody02WalkEndAnimationFile[];
extern u32 gHanachanHeadWalkEndAnimationFile[];

#pragma define_section data_ov034_0211389c ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov034_0211389c begin
void *data_ov034_0211389c[3] = {
    (void *)&gHanachanHeadWalkEndAnimationFile,
    (void *)&gHanachanBody01WalkEndAnimationFile,
    (void *)&gHanachanBody02WalkEndAnimationFile,
};
#pragma section data_ov034_0211389c end
