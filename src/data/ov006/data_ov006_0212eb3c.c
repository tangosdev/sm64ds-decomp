/* Byte table, 8 bytes. */
#include "types.h"

#pragma define_section obj ".rodata.gap3" ".rodata.gap3" ".rodata.gap3" far_data
#pragma section obj begin
unsigned char sPachinkoPipeIdleTimes[8] = {
    0x04, 0x02, 0x03, 0x02, 0x04, 0x02, 0x03, 0x02,
};
#pragma section obj end
