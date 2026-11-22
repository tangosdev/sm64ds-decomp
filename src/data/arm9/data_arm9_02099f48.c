/* claimed-storage */
/* arm9 .data 0x02099f48..0x02099fd4. 20 storage addresses, one object. */
#include "types.h"

extern void _ZN7fBase_c11executePackEv(void);
extern void _ZN7fBase_c10deletePackEv(void);
extern void _ZN7fBase_c11connectProcEv(void);
extern void _ZN7fBase_c8drawPackEv(void);
extern void _ZN7fBase_c10createPackEv(void);
extern void SPLUtil_DrawXYPlane(void);
extern void SPLUtil_DrawXZPlane(void);
extern void SPLUtil_RotateY(void);
extern void SPLUtil_RotateXYZ(void);

#pragma explicit_zero_data on
u32 data_02099f48[] = {
    (u32)&_ZN7fBase_c11executePackEv,
    0x00000000,
    (u32)&_ZN7fBase_c10deletePackEv,
    0x00000000, 0x43455845, 0x00455455,
    (u32)&_ZN7fBase_c11connectProcEv,
    0x00000000,
    (u32)&_ZN7fBase_c8drawPackEv,
    0x00000000,
    (u32)&_ZN7fBase_c10createPackEv,
    0x00000000, 0x4e4e4f43, 0x00544345, 0x00040201, 0x00040201, 0x00001000, 0x00001000, 0x00001000,
    0x00080004, 0x00200010, 0x00800040, 0x02000100, 0x0000000f, 0x0000005a, 0x00000226, 0x00000004,
    (u32)&SPLUtil_DrawXYPlane,
    (u32)&SPLUtil_DrawXZPlane,
    (u32)&SPLUtil_RotateY,
    (u32)&SPLUtil_RotateXYZ,
    0x10000000, 0x00000000, 0x00000001, 0x00000003,
};
#pragma explicit_zero_data off
