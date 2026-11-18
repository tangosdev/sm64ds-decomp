/* ov034 cartridge data at 0x02113860, 20 bytes. */
#include "types.h"

extern u32 gHanachanBody01WaitAnimationFile[];
extern u32 gHanachanBody02WaitAnimationFile[];
extern u32 gHanachanBody03WaitAnimationFile[];
extern u32 gHanachanBody04WaitAnimationFile[];
extern u32 gHanachanHeadWaitAnimationFile[];

#pragma define_section data_ov034_02113860 ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov034_02113860 begin
void *data_ov034_02113860[5] = {
    (void *)&gHanachanHeadWaitAnimationFile,   (void *)&gHanachanBody01WaitAnimationFile,
    (void *)&gHanachanBody02WaitAnimationFile, (void *)&gHanachanBody03WaitAnimationFile,
    (void *)&gHanachanBody04WaitAnimationFile,
};
#pragma section data_ov034_02113860 end
