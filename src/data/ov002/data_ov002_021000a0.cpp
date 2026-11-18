//cpp
/* .rodata data at 0x021000a0; bytes from the cartridge. */
#include "types.h"

struct SharedFilePtr;

extern struct SharedFilePtr data_ov002_0210e7d8;  /* local extern: linker-name reference for a data word */
extern struct SharedFilePtr data_ov002_0210e3a0;  /* local extern: linker-name reference for a data word */

extern char * const data_ov002_021000a0[2] = {
    (char *)&data_ov002_0210e7d8, (char *)&data_ov002_0210e3a0
};
