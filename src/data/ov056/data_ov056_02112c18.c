/* ov056 cartridge data at 0x02112c18, 12 bytes. */
#include "types.h"

extern u32 data_ov056_02112e84[];
extern u32 data_ov056_02113054[];

u32 data_ov056_02112c18[3] = {
    (u32)&data_ov056_02113054,
    0x00003a05,
    (u32)&data_ov056_02112e84,
};
