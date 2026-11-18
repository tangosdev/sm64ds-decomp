/* ov029 cartridge data at 0x02113be8, 12 bytes. */
#include "types.h"

extern u32 data_ov029_0211300c[];
extern u32 gWcObj01CollisionFile[];
extern u32 gWcObj01ModelFile[];

u32 data_ov029_02113be8[3] = {
    (u32)&gWcObj01ModelFile, (u32)&gWcObj01CollisionFile, (u32)&data_ov029_0211300c,
};
