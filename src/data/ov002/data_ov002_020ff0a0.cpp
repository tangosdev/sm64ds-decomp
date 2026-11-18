//cpp
/* .rodata data at 0x020ff0a0; bytes from the cartridge. */
#include "types.h"

struct daObjMarioCapFileHandle;

extern struct daObjMarioCapFileHandle
    data_ov002_0210de48; /* local extern: linker-name reference for a data word */
extern struct daObjMarioCapFileHandle
    data_ov002_0210de60; /* local extern: linker-name reference for a data word */
extern struct daObjMarioCapFileHandle
    data_ov002_0210de50; /* local extern: linker-name reference for a data word */

extern struct SharedFilePtr *const data_ov002_020ff0a0[3] = {
    (struct SharedFilePtr *)&data_ov002_0210de48, (struct SharedFilePtr *)&data_ov002_0210de60,
    (struct SharedFilePtr *)&data_ov002_0210de50};
