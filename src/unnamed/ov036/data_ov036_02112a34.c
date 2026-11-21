#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

#pragma define_section s ".data.data_ov036_02112a00" ".data.data_ov036_02112a00" ".data.data_ov036_02112a00" far_data
#pragma section s begin
u32 data_ov036_02112a34[4] = {
    0x0ce4effc,
    0x0080f542,
    0x0905e50c,
    0x0040ff9c,
};

u32 data_ov036_02112a44[4] = {
    0x0b220014,
    0x05dcf902,
    0xeaab0003,
    0xffff0000,
};

#pragma section s end
