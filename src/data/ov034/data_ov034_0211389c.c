/* ov034 cartridge data at 0x0211389c, 12 bytes. */
#include "types.h"

extern u32 gHanachanBody01WalkEndAnimationFile[];
extern u32 gHanachanBody02WalkEndAnimationFile[];
extern u32 gHanachanHeadWalkEndAnimationFile[];

const u32 data_ov034_0211389c[3] = {
    (u32)&gHanachanHeadWalkEndAnimationFile, (u32)&gHanachanBody01WalkEndAnimationFile, (u32)&gHanachanBody02WalkEndAnimationFile,
};
