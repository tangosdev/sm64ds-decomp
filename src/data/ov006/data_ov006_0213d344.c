/* 16-bit table, 6 entries. */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
unsigned short sMemory2FlipFrameTimes[6] = {
    0x0008, 0x0006, 0x0004, 0x0006, 0x0018, 0x0018,
};
#pragma section obj end
