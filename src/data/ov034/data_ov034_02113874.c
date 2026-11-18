/* ov034 cartridge data at 0x02113874, 20 bytes. */
#include "types.h"

extern u32 gHanachanBody01WalkAnimationFile[];
extern u32 gHanachanBody02WalkAnimationFile[];
extern u32 gHanachanBody03WalkAnimationFile[];
extern u32 gHanachanBody04WalkAnimationFile[];
extern u32 gHanachanHeadWalkAnimationFile[];

const u32 data_ov034_02113874[5] = {
    (u32)&gHanachanHeadWalkAnimationFile, (u32)&gHanachanBody01WalkAnimationFile, (u32)&gHanachanBody02WalkAnimationFile, (u32)&gHanachanBody03WalkAnimationFile, (u32)&gHanachanBody04WalkAnimationFile,
};
