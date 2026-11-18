//cpp
/* .data data at 0x0210aa40; bytes from the cartridge. */
#include "types.h"

struct daStar_c;

extern "C" struct daStar_c *
_Z23daStar_c_classInit_STARv(); /* local extern: linker-name reference for a data word */

unsigned int g_profile_STAR[7] = {(u32)_Z23daStar_c_classInit_STARv,
                                  0x004000b2,
                                  0x20000002,
                                  0x00032000,
                                  0x001f4000,
                                  0x01000000,
                                  0x01000000};
