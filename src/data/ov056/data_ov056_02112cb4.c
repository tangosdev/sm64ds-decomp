/* ov056 cartridge data at 0x02112cb4, 20 bytes. */
#include "types.h"

extern u32 data_ov056_02112cf4[];
extern u32 data_ov056_02112dc4[];

u32 data_ov056_02112cb4[5] = {
    0x00002002, (u32)&data_ov056_02112dc4, 0x00000d03, (u32)&data_ov056_02112cf4, 0x00000304,
};
