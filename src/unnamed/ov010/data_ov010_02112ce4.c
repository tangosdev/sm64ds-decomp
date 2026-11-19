/* Zero words between the last initialized object and .bss.
 * mwccarm puts an all-zero object in .bss unless this pragma is on. */
#include "types.h"

#pragma explicit_zero_data on
u32 data_ov010_02112ce4[7] = {0, 0, 0, 0, 0, 0, 0};
