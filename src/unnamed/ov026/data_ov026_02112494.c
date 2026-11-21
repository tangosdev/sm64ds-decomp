#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

/* A class section sits between this run and .rodata, so the objects share one named section. */
/* explicit_zero_data keeps the zero word in this section. Without it mwcc emits the word as BSS. */

#pragma explicit_zero_data on
#pragma define_section s ".rodata.data_ov026_02112494" ".rodata.data_ov026_02112494" ".rodata.data_ov026_02112494" far_data
#pragma section s begin
u32 data_ov026_0211249c[1] = {
    0x00001000,
};

u32 data_ov026_02112498[1] = {
    0x00001000,
};

u32 data_ov026_02112494[1] = {
    0x00000000,
};

u32 data_ov026_021124a0[1] = {
    0x0000fad8,
};

#pragma section s end
