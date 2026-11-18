/* ov014 cartridge data at 0x021145c4, 12 bytes. */
#include "types.h"

extern u32 data_ov014_02114578[];
extern u32 gBAnaShutterCollisionFile[];
extern u32 gBAnaShutterModelFile[];

struct SharedFilePtr;
struct CLPS_Block;

/* Layout mirrored from include/ResourceDescriptor.h, which is C++-only. */
struct ResourceDescriptor {
    struct SharedFilePtr *model;
    struct SharedFilePtr *collision;
    struct CLPS_Block *clps;
};

#pragma define_section data_ov014_021145c4 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021145c4 begin
struct ResourceDescriptor data_ov014_021145c4 = {
    (struct SharedFilePtr *)&gBAnaShutterModelFile,
    (struct SharedFilePtr *)&gBAnaShutterCollisionFile,
    (struct CLPS_Block *)&data_ov014_02114578,
};
#pragma section data_ov014_021145c4 end
