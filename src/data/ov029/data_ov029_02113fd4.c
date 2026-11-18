/* ov029 cartridge data at 0x02113fd4, 12 bytes. */
#include "types.h"

extern u32 data_ov029_021130ac[];
extern u32 gWcObj07CollisionFile[];
extern u32 gWcObj07ModelFile[];

u32 data_ov029_02113fd4[3] = {
    (u32)&gWcObj07ModelFile, (u32)&gWcObj07CollisionFile, (u32)&data_ov029_021130ac,
};
