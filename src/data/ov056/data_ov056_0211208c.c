/* ov056 cartridge data at 0x0211208c, 24 bytes. */
#include "types.h"

#pragma define_section data_ov056_0211208c ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov056_0211208c begin
#include "private/BTA_File.h"

BTA_File data_ov056_0211208c = {
    0x2ed1,
    0,
    (fx32 *)0x00002eef,
    (u16 *)0x00002f0d,
    (fx32 *)0x00002f2c,
    0x2f4a,
    0,
    (BTAMaterialRecord *)0x00002f68,
};
#pragma section data_ov056_0211208c end
