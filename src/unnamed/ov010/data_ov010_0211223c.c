#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

#pragma define_section s ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" far_data
#pragma section s begin
u32 data_ov010_0211223c[3] = {
    0x000006e9,
    0x000006eb,
    0x000006ed,
};

u32 data_ov010_02112248[4] = {
    0x08980898,
    0x08980898,
    0x08980898,
    0x00000898,
};

#pragma section s end
