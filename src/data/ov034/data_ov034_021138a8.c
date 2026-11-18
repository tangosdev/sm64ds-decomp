/* ov034 cartridge data at 0x021138a8, 8 bytes. */
#include "types.h"

extern u32 gHanachanBody03WalkEndAnimationFile[];
extern u32 gHanachanBody04WalkEndAnimationFile[];

const u32 data_ov034_021138a8[2] = {
    (u32)&gHanachanBody03WalkEndAnimationFile,
    (u32)&gHanachanBody04WalkEndAnimationFile,
};
