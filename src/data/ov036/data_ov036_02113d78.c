/* ov036 cartridge data at 0x02113d78, 12 bytes. */
#include "types.h"

extern u32 data_ov036_02112ac8[];
extern u32 gRcGuruguruCollisionFile[];
extern u32 gRcGuruguruModelFile[];

u32 data_ov036_02113d78[3] = {
    (u32)&gRcGuruguruModelFile, (u32)&gRcGuruguruCollisionFile, (u32)&data_ov036_02112ac8,
};
