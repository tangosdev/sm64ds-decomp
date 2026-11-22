/* claimed-storage */
/* arm9 .data 0x020992e4..0x020992f8. 2 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi17__class_type_infoE[];
extern u32 _ZTS8dM3dGSph[];

#pragma explicit_zero_data on
u32 _ZTI8dM3dGSph[] = {
    (u32)&_ZTVN3abi17__class_type_infoE,
    (u32)&_ZTS8dM3dGSph,
    0x334d6438, 0x70534764, 0x00000068,
};
#pragma explicit_zero_data off
