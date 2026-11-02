#ifndef PRIVATE_CLPS_ROW_H
#define PRIVATE_CLPS_ROW_H

#include "types.h"

struct CLPS_Block;

/* One 0xc-stride row of a CLPS resource table (daObjBlockL_c's
 * data_ov002_02108ab8, daObjSimpleBg_c's data_ov102_0214e190): the first word
 * points at the CLPS block; the rest of the row is read by nothing here. */
struct ClpsRow {
    struct CLPS_Block *block; /* 0x00 */
    u8 pad[8];
};

#ifndef SM64DS_PLATFORM_PC
typedef char ClpsRow_size_must_be_0xc[sizeof(struct ClpsRow) == 0xc ? 1 : -1];
#endif

#endif
