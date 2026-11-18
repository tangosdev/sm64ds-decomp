//cpp
/* .rodata data at 0x02112c0c; bytes from the cartridge. */
#include "types.h"

struct PgMthrAnimFileResHandle;

extern struct PgMthrAnimFileResHandle
    gPenguinWait1AnimationFile; /* local extern: linker-name reference for a data word */
extern struct PgMthrAnimFileResHandle
    gPenguinWalkAnimationFile; /* local extern: linker-name reference for a data word */

extern int const data_ov018_02112c0c[2] = {(int)&gPenguinWait1AnimationFile,
                                           (int)&gPenguinWalkAnimationFile};
