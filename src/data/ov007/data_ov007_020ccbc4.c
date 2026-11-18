/* 0x020ccbc4: the nine item layout rows (Ov007ItemLayout[9]) and 2 tail bytes.
 * const (rodata). */
#include "types.h"
#include "Ov007Item.h"

#pragma define_section sec_020ccbc4 ".rodata" ".rodata" ".rodata" far_data
#pragma section sec_020ccbc4 begin
typedef struct Ov007ItemLayoutTable {
    Ov007ItemLayout rows[9];
    u8 pad_5a[2];
} Ov007ItemLayoutTable;
Ov007ItemLayoutTable data_ov007_020ccbc4 = {
    { /* rows[9] */
        {1, 12, 176, 0, 64},
        {1, 128, 164, 0, 64},
        {1, 232, 11, 0, -32},
        {5, 33, 93, 0, 0},
        {5, 162, 61, 0, 0},
        {5, 230, 78, 0, 0},
        {5, 213, 67, 0, 0},
        {7, 14, 40, 0, -256},
        {9, 128, 136, 0, 100},
    },
    {0, 0},
};
#pragma section sec_020ccbc4 end
