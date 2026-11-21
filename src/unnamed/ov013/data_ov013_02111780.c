#include "types.h"

extern u32 data_ov013_02111804[];

u32 data_ov013_02111780[1] = {
    0x00000004,
};

u32 data_ov013_02111784[2] = {
    0x00000002,
    (u32)&data_ov013_02111804,
};
