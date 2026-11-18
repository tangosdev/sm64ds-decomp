/* ov029 cartridge data at 0x02113be8, 12 bytes. */
#include "types.h"

extern u32 data_ov029_0211300c[];
extern u32 gWcObj01CollisionFile[];
extern u32 gWcObj01ModelFile[];

struct SharedFilePtr;
struct CLPS_Block;

/* Layout from include/daObjFloatBoard_c.h (defined under __cplusplus only). */
struct daObjFloatBoard_c_Resources {
    struct SharedFilePtr *model;
    struct SharedFilePtr *collision;
    struct CLPS_Block *clps;
};

struct daObjFloatBoard_c_Resources data_ov029_02113be8 = {
    (struct SharedFilePtr *)&gWcObj01ModelFile,
    (struct SharedFilePtr *)&gWcObj01CollisionFile,
    (struct CLPS_Block *)&data_ov029_0211300c,
};
