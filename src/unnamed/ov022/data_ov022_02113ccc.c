#include "types.h"

extern u32 data_ov064_0211bbac[];

extern u32 gFlRingCollisionFile[];

#pragma define_section s_02113ccc ".data.data_ov022_02113ccc" ".data.data_ov022_02113ccc" ".data.data_ov022_02113ccc" far_data
#pragma section s_02113ccc begin
u32 data_ov022_02113ccc[2] = {
    (u32)&gFlRingCollisionFile,
    (u32)data_ov064_0211bbac,
};
#pragma section s_02113ccc end
