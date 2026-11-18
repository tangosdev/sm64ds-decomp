/* ov034 cartridge data at 0x021138bc, 8 bytes. */
#include "types.h"

extern u32 gHanachanBody03DamageAnimationFile[];
extern u32 gHanachanBody04DamageAnimationFile[];

const u32 data_ov034_021138bc[2] = {
    (u32)&gHanachanBody03DamageAnimationFile,
    (u32)&gHanachanBody04DamageAnimationFile,
};
