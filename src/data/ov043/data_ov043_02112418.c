/* ov043 cartridge data at 0x02112418, 4 bytes. */
#include "types.h"

extern u32 gKm1KurumaModelFile[];

u32 data_ov043_02112418[1] = {
    (u32)&gKm1KurumaModelFile,
};
