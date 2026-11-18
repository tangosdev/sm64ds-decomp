//cpp
/* .data data at 0x02108518; bytes from the cartridge. */
#include "types.h"

struct daCamTag_c;

extern "C" struct daCamTag_c *
_Z20daCamTag_c_classInitv();            /* local extern: linker-name reference for a data word */
extern unsigned int _ZTI10daCamTag_c[]; /* local extern: linker-name reference for a data word */

unsigned int g_profile_CAMERA_TAG[9] = {(u32)_Z20daCamTag_c_classInitv,
                                        0x0001014d,
                                        0x00000000,
                                        0x00000000,
                                        0x00000000,
                                        0x00000000,
                                        0x00000000,
                                        0x00000000,
                                        (u32)_ZTI10daCamTag_c};
