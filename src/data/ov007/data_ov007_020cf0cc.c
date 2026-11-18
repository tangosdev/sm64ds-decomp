/* 0x020cf0cc: BTA material records of data_ov007_020cd9e4.
 * 3 BTAMaterialRecord rows, const (rodata). */
#include "types.h"
#include "private/BTA_File.h"

#pragma define_section sec_020cf0cc ".rodata.gap0" ".rodata.gap0" ".rodata.gap0" far_data
#pragma section sec_020cf0cc begin
extern char data_ov007_020d7e00[];
extern char data_ov007_020d7e08[];
extern char data_ov007_020d7e10[];
BTAMaterialRecord data_ov007_020cf0cc[3] = {
    {0xffff, 0, &data_ov007_020d7e00[0], 41, 0, 41, 0, 1, 0, 1, 0, 1, 1},
    {0xffff, 0, &data_ov007_020d7e08[0], 1, 41, 1, 41, 1, 0, 41, 2, 41, 43},
    {0xffff, 0, &data_ov007_020d7e10[0], 1, 41, 1, 41, 1, 0, 41, 84, 41, 125},
};
#pragma section sec_020cf0cc end
