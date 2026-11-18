//cpp
/* .data data at 0x0210aa94; bytes from the cartridge. */
#include "types.h"

extern "C" void *_Z16StarCamera_Spawnv(); /* local extern: linker-name reference for a data word */
extern unsigned int _ZTI12daStarBase_c[]; /* local extern: linker-name reference for a data word */

unsigned int g_profile_STAR_CAMERA[9] = {(u32)_Z16StarCamera_Spawnv,
                                         0x003f00b1,
                                         0x00000000,
                                         0x00000000,
                                         0x000c8000,
                                         0x00000000,
                                         0x00000000,
                                         0x00000000,
                                         (u32)_ZTI12daStarBase_c};
