//cpp
/* .data data at 0x0210845c; bytes from the cartridge. */
#include "types.h"

struct daBar_c;

extern "C" struct daBar_c *
_Z17daBar_c_classInitv();           /* local extern: linker-name reference for a data word */
extern unsigned int _ZTI7daBar_c[]; /* local extern: linker-name reference for a data word */

unsigned int g_profile_BAR[9] = {(u32)_Z17daBar_c_classInitv,
                                 0x0099011f,
                                 0x00000003,
                                 0x00000000,
                                 0x00000000,
                                 0x00000000,
                                 0x00000000,
                                 0x00000000,
                                 (u32)_ZTI7daBar_c};
