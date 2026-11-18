/* ov029 cartridge data at 0x021131b4, 80 bytes. */
#include "types.h"

extern u32 data_ov029_02112f08[];
extern u32 data_ov029_02112f18[];
extern u32 data_ov029_02112f28[];
extern u32 data_ov029_02112f38[];
extern u32 data_ov029_02112f48[];
extern u32 data_ov029_02112f58[];
extern u32 data_ov029_02112f68[];
extern u32 data_ov029_021130cc[];
extern u32 data_ov029_021132c4[];
extern u32 data_ov029_021136cc[];

u32 data_ov029_021131b4[20] = {
    0x00005100, (u32)&data_ov029_021136cc, 0x00002205, (u32)&data_ov029_021132c4,
    0x00000120, (u32)&data_ov029_02112f08, 0x00000140, (u32)&data_ov029_02112f68,
    0x00000160, (u32)&data_ov029_02112f38, 0x00000180, (u32)&data_ov029_02112f28,
    0x000001a0, (u32)&data_ov029_02112f48, 0x000001c0, (u32)&data_ov029_02112f18,
    0x000001e0, (u32)&data_ov029_02112f58, 0x000006e5, (u32)&data_ov029_021130cc,
};
