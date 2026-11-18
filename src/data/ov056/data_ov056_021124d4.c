/* ov056 cartridge data at 0x021124d4, 24 bytes. */
#include "types.h"

#pragma define_section data_ov056_021124d4 ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov056_021124d4 begin
int *data_ov056_021124d4[6] = {
    (int *)0x00008889, (int *)0x0000899a, (int *)0x00008aab,
    (int *)0x00008bbc, (int *)0x00008ccd, (int *)0x00008dde,
};
#pragma section data_ov056_021124d4 end
