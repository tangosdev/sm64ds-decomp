/* 0x020cf024: BTA material records of data_ov007_020cda2c.
 * 3 BTAMaterialRecord rows, const (rodata). */
#include "types.h"
#include "private/BTA_File.h"

#pragma define_section sec_020cf024 ".rodata.gap0" ".rodata.gap0" ".rodata.gap0" far_data
#pragma section sec_020cf024 begin
extern char data_ov007_020d7e00[];
extern char data_ov007_020d7e08[];
extern char data_ov007_020d7e10[];
BTAMaterialRecord data_ov007_020cf024[3] = {
    {0xffff, 0, &data_ov007_020d7e00[0], 852, 0, 852, 0, 1, 0, 1, 0, 1, 1},
    {0xffff, 0, &data_ov007_020d7e08[0], 1, 0, 1, 0, 1, 0, 852, 2, 852, 854},
    {0xffff, 0, &data_ov007_020d7e10[0], 1, 0, 1, 0, 1, 0, 852, 1706, 852, 2558},
};
#pragma section sec_020cf024 end
