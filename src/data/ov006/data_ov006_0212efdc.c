/* 16-bit table, 8 entries. */
#include "types.h"

#pragma define_section obj ".rodata.after_sound" ".rodata.after_sound" ".rodata.after_sound" far_data
#pragma section obj begin
unsigned short sTeresaBg3FlashTimes[8] = {
    0x0018, 0x0018, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004,
};
#pragma section obj end
