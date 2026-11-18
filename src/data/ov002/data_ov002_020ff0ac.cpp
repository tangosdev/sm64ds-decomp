//cpp
/* .rodata data at 0x020ff0ac; bytes from the cartridge. */
#include "types.h"

struct SharedFilePtr;

extern struct SharedFilePtr data_ov002_0210da40;  /* local extern: linker-name reference for a data word */
extern struct SharedFilePtr data_ov002_0210d9a0;  /* local extern: linker-name reference for a data word */
extern struct SharedFilePtr data_ov002_0210d9c0;  /* local extern: linker-name reference for a data word */

extern void * const data_ov002_020ff0ac[3] = {
    (void *)&data_ov002_0210da40, (void *)&data_ov002_0210d9a0, (void *)&data_ov002_0210d9c0
};
