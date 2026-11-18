/* ov043 cartridge data at 0x0211241c, 8 bytes. */
#include "types.h"

extern u32 data_ov043_02111b80[];
extern u32 gKm1KurumaCollisionFile[];

u32 data_ov043_0211241c[2] = {
    (u32)&gKm1KurumaCollisionFile,
    (u32)&data_ov043_02111b80,
};
