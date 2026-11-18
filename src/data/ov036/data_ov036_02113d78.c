/* ov036 cartridge data at 0x02113d78, 12 bytes. */
#include "types.h"

extern u32 data_ov036_02112ac8[];
extern u32 gRcGuruguruCollisionFile[];
extern u32 gRcGuruguruModelFile[];

struct SharedFilePtr;
struct CLPS_Block;

/* Layout from d_a_obj_rc_guruguru.cpp, which defines the type locally. */
struct RcGuruguruAssets {
    struct SharedFilePtr *modelFile;
    struct SharedFilePtr *clsnFile;
    struct CLPS_Block *clps;
};

struct RcGuruguruAssets data_ov036_02113d78 = {
    (struct SharedFilePtr *)&gRcGuruguruModelFile,
    (struct SharedFilePtr *)&gRcGuruguruCollisionFile,
    (struct CLPS_Block *)&data_ov036_02112ac8,
};
