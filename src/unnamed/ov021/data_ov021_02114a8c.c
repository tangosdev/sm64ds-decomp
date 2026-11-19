/* Overlay 21 leftover .bss: five zero words. The .data run at 0x02114860
 * is class vtable data and stays unclaimed. */
#include "types.h"

/* 0x02114a8c..0x02114aa0 (20 bytes, 5 words) */
u32 data_ov021_02114a8c[5];
