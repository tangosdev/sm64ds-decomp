/* 0x020ccc20: layout slot -> target slot table, indexed [selected * 3 + target]. */
#include "types.h"

#pragma define_section sec_020ccc20 ".rodata" ".rodata" ".rodata" far_data
#pragma section sec_020ccc20 begin
int data_ov007_020ccc20[9] = {
    0, 11, 12, 13, 0, 14, 15, 16, 0,
};
#pragma section sec_020ccc20 end
