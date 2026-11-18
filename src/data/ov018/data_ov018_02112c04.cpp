//cpp
/* .rodata data at 0x02112c04; bytes from the cartridge. */
#include "types.h"

struct PgMthrTexSeqFileHandle;

extern struct PgMthrTexSeqFileHandle gPenguinAngryTextureSequenceFile;  /* local extern: linker-name reference for a data word */

extern struct SharedFilePtr * const data_ov018_02112c04[1] = {
    (struct SharedFilePtr *)&gPenguinAngryTextureSequenceFile
};
