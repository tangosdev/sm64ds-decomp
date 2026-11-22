/* claimed-storage */
/* ov077 .rodata 0x02127230..0x02127240. 2 storage addresses, one object. */
#include "types.h"

extern u32 data_ov077_02127b20[];
extern u32 data_ov077_02127b30[];
extern u32 data_ov077_02127b28[];
extern u32 data_ov077_02127b40[];

#pragma explicit_zero_data on
const u32 data_ov077_02127230[] = {
    (u32)&data_ov077_02127b20,
    (u32)&data_ov077_02127b30,
    (u32)&data_ov077_02127b28,
    (u32)&data_ov077_02127b40,
};
#pragma explicit_zero_data off
