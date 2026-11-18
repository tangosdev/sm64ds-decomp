/* ov029 cartridge data at 0x02113fd4, 12 bytes. */
#include "types.h"

extern u32 data_ov029_021130ac[];
extern u32 gWcObj07CollisionFile[];
extern u32 gWcObj07ModelFile[];

struct SharedFilePtr;
struct CLPS_Block;

/* Layout mirrored from include/ResourceDescriptor.h, which is C++-only. */
struct ResourceDescriptor {
    struct SharedFilePtr *model;
    struct SharedFilePtr *collision;
    struct CLPS_Block *clps;
};

struct ResourceDescriptor data_ov029_02113fd4 = {
    (struct SharedFilePtr *)&gWcObj07ModelFile,
    (struct SharedFilePtr *)&gWcObj07CollisionFile,
    (struct CLPS_Block *)&data_ov029_021130ac,
};
