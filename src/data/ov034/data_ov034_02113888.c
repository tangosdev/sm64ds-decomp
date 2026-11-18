/* ov034 cartridge data at 0x02113888, 20 bytes. */
#include "types.h"

extern u32 gHanachanBody01WalkStartAnimationFile[];
extern u32 gHanachanBody02WalkStartAnimationFile[];
extern u32 gHanachanBody03WalkStartAnimationFile[];
extern u32 gHanachanBody04WalkStartAnimationFile[];
extern u32 gHanachanHeadWalkStartAnimationFile[];

const u32 data_ov034_02113888[5] = {
    (u32)&gHanachanHeadWalkStartAnimationFile, (u32)&gHanachanBody01WalkStartAnimationFile, (u32)&gHanachanBody02WalkStartAnimationFile, (u32)&gHanachanBody03WalkStartAnimationFile, (u32)&gHanachanBody04WalkStartAnimationFile,
};
