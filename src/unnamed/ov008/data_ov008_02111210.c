#include "types.h"

extern u32 data_ov008_02111508[];
extern u32 data_ov008_021111d0[];
extern u32 data_ov008_021111e0[];

u32 data_ov008_02111210[2] = {
    (u32)data_ov008_02111508,
    (u32)data_ov008_021111d0,
};

u32 data_ov008_02111218[4] = {
    0x07d507d7,
    0x07d907d8,
    (u32)data_ov008_021111e0,
    0x06400101,
};
