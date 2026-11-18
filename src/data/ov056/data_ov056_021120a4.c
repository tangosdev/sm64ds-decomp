/* ov056 cartridge data at 0x021120a4, 24 bytes. */
#include "types.h"

#pragma define_section data_ov056_021120a4 ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov056_021120a4 begin
#include "private/BTA_File.h"

BTA_File data_ov056_021120a4 = {
    0x2f87,
    0,
    (fx32 *)0x00002fa5,
    (u16 *)0x00002fc3,
    (fx32 *)0x00002fe2,
    0x3000,
    0,
    (BTAMaterialRecord *)0x0000301e,
};
#pragma section data_ov056_021120a4 end
