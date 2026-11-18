/* ov029 cartridge data at 0x02113174, 64 bytes. */
#include "types.h"

extern u32 data_ov029_02112ea0[];
extern u32 data_ov029_02112ea4[];
extern u32 data_ov029_02112ecc[];
extern u32 data_ov029_02112ee8[];
extern u32 data_ov029_02112ef8[];
extern u32 data_ov029_02112f78[];
extern u32 data_ov029_021130fc[];
extern u32 data_ov029_02113134[];

u32 data_ov029_02113174[16] = {
    0x00000404, (u32)&data_ov029_021130fc, 0x0000020b, (u32)&data_ov029_02112ea4,
    0x00000401, (u32)&data_ov029_02113134, 0x00000206, (u32)&data_ov029_02112ef8,
    0x00000207, (u32)&data_ov029_02112f78, 0x00000208, (u32)&data_ov029_02112ee8,
    0x00000109, (u32)&data_ov029_02112ecc, 0x0000010e, (u32)&data_ov029_02112ea0,
};
