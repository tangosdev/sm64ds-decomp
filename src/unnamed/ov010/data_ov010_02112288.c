#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

#pragma define_section s ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" far_data
#pragma section s begin
u32 data_ov010_02112288[3] = {
    0xff0e0000,
    0x000effff,
    0xffffff0c,
};

#pragma section s end
