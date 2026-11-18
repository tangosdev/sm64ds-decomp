/* Byte table, 8 bytes. */
#include "types.h"

#pragma define_section obj ".rodata.gap3" ".rodata.gap3" ".rodata.gap3" far_data
#pragma section obj begin
unsigned char sPachinkoPipeIdleFrames[8] = {
    0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0d, 0x0c,
};
#pragma section obj end
