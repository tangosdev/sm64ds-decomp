/* ov056 cartridge data at 0x02112c34, 16 bytes. */
#include "types.h"

extern u32 data_ov056_021115cc[];
extern u32 data_ov056_02111624[];
extern int data_ov056_02111a60[];

u32 data_ov056_02112c34[4] = {
    0x0000021d,
    (u32)&data_ov056_021115cc,
    (u32)&data_ov056_02111624,
    (u32)&data_ov056_02111a60,
};
