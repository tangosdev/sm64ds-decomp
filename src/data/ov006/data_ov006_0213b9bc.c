/* Byte table, 4 bytes. */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
unsigned char sBomroomBombFrameCounts[4] = {
    0x02, 0x04, 0x04, 0x06,
};
#pragma section obj end
