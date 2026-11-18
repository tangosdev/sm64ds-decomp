/* Byte table, 8 bytes. */
#include "types.h"

#pragma define_section obj ".rodata.after_sound" ".rodata.after_sound" ".rodata.after_sound" far_data
#pragma section obj begin
unsigned char sTeresaRevealSlideFrames[8] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x03, 0x02, 0x01,
};
#pragma section obj end
