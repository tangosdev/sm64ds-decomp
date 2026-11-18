//cpp
/* .data data at 0x02108884; bytes from the cartridge. */
#include "types.h"

struct daFeather_c;

extern "C" struct daFeather_c *
_Z21daFeather_c_classInitv();            /* local extern: linker-name reference for a data word */
extern unsigned int _ZTI11daFeather_c[]; /* local extern: linker-name reference for a data word */

unsigned int g_profile_FEATHER[9] = {(u32)_Z21daFeather_c_classInitv,
                                     0x01590159,
                                     0x00000002,
                                     0x00060000,
                                     0x00200000,
                                     0x01000000,
                                     0x01000000,
                                     0x00000000,
                                     (u32)_ZTI11daFeather_c};
