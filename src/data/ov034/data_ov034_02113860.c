/* ov034 cartridge data at 0x02113860, 20 bytes. */
#include "types.h"

extern u32 gHanachanBody01WaitAnimationFile[];
extern u32 gHanachanBody02WaitAnimationFile[];
extern u32 gHanachanBody03WaitAnimationFile[];
extern u32 gHanachanBody04WaitAnimationFile[];
extern u32 gHanachanHeadWaitAnimationFile[];

const u32 data_ov034_02113860[5] = {
    (u32)&gHanachanHeadWaitAnimationFile, (u32)&gHanachanBody01WaitAnimationFile, (u32)&gHanachanBody02WaitAnimationFile, (u32)&gHanachanBody03WaitAnimationFile, (u32)&gHanachanBody04WaitAnimationFile,
};
