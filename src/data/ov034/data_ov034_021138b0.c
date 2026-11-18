/* ov034 cartridge data at 0x021138b0, 12 bytes. */
#include "types.h"

extern u32 gHanachanBody01DamageAnimationFile[];
extern u32 gHanachanBody02DamageAnimationFile[];
extern u32 gHanachanHeadDamageAnimationFile[];

const u32 data_ov034_021138b0[3] = {
    (u32)&gHanachanHeadDamageAnimationFile, (u32)&gHanachanBody01DamageAnimationFile, (u32)&gHanachanBody02DamageAnimationFile,
};
