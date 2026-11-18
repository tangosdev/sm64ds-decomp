/* ov034 cartridge data at 0x021138b0, 12 bytes. */
#include "types.h"

extern u32 gHanachanBody01DamageAnimationFile[];
extern u32 gHanachanBody02DamageAnimationFile[];
extern u32 gHanachanHeadDamageAnimationFile[];

#pragma define_section data_ov034_021138b0 ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov034_021138b0 begin
void *data_ov034_021138b0[3] = {
    (void *)&gHanachanHeadDamageAnimationFile,
    (void *)&gHanachanBody01DamageAnimationFile,
    (void *)&gHanachanBody02DamageAnimationFile,
};
#pragma section data_ov034_021138b0 end
