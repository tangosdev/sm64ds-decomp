/* claimed-storage */
/* arm9 .data 0x02099360..0x02099370. 2 storage addresses, one object. */
#include "types.h"

extern void _ZN5dBgPiD1Ev(void);
extern void _ZN5dBgPiD0Ev(void);
extern u32 _ZTI5dBgPi[];

#pragma explicit_zero_data on
u32 data_02099360[] = {
    0x00000000,
    (u32)&_ZTI5dBgPi,
    (u32)&_ZN5dBgPiD1Ev,
    (u32)&_ZN5dBgPiD0Ev,
};
#pragma explicit_zero_data off
