//cpp
/* .rodata data at 0x020ff028; bytes from the cartridge. */
#include "types.h"

struct SharedFilePtr;

extern struct SharedFilePtr data_ov002_0210da40;  /* local extern: linker-name reference for a data word */
extern struct SharedFilePtr data_ov002_0210d9a0;  /* local extern: linker-name reference for a data word */
extern struct SharedFilePtr data_ov002_0210d9c0;  /* local extern: linker-name reference for a data word */

extern struct SharedFilePtr * const data_ov002_020ff028[6] = {
    (struct SharedFilePtr *)&data_ov002_0210da40, (struct SharedFilePtr *)&data_ov002_0210d9a0, (struct SharedFilePtr *)&data_ov002_0210d9c0, 0x00000000, 0x00000000, 0x00000000
};
