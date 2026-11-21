#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov009_02112b6c[];
extern u32 data_ov009_02112bac[];

#pragma define_section s ".data.data_ov009_02112b60" ".data.data_ov009_02112b60" ".data.data_ov009_02112b60" far_data
#pragma section s begin
u32 data_ov009_02112b84[1] = {
    0x00000074,
};

u32 data_ov009_02112b88[3] = {
    (u32)&data_ov009_02112b6c,
    (u32)&data_ov009_02112bac,
    0x3a980000,
};

u32 data_ov009_02112b94[5] = {
    0x17a8ea36,
    0x181cfb50,
    0xf09ce636,
    0xf604fc73,
    0x158a3e37,
};

#pragma section s end
