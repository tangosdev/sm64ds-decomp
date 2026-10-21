#include "types.h"

/* Stroke counts for the six portraits. This table sits at 0x020cdb34, which is
 * earlier in ov007 .rodata than the function that reads it, so it cannot live
 * in that function's translation unit. */
#pragma define_section forcerodata ".rodata" ".rodata" ".rodata" data_mode=far_abs
#pragma section forcerodata begin
const int sPortraitStrokeCounts[6] = {30, 34, 36, 20, 32, 32};
#pragma section forcerodata end
