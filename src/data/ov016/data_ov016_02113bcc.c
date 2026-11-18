/* ov016 cartridge data at 0x02113bcc, 32 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113bcc ".data.hasira_head"                                     \
                                           ".data.hasira_head"                                     \
                                           ".data.hasira_head" far_data
#pragma section data_ov016_02113bcc begin
u32 data_ov016_02113bcc[8] = {
    0x0ed800fe, 0x0672ec78, 0x15550000, 0x20040000, 0xec140014, 0xff06044c, 0xd55600ff, 0x0b260000,
};
#pragma section data_ov016_02113bcc end
