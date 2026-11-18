/* 16-bit table, 2 entries. */
#include "types.h"

#pragma define_section obj ".rodata.after_sound" ".rodata.after_sound" ".rodata.after_sound" far_data
#pragma section obj begin
unsigned short sTeresaBigBooFrameTimes[2] = {
    0x0008, 0x000c,
};
#pragma section obj end
