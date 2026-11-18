/* 0x020cf078: BTA material records of data_ov007_020cda5c.
 * 3 BTAMaterialRecord rows, const (rodata). */
#include "types.h"
#include "private/BTA_File.h"

#pragma define_section sec_020cf078 ".rodata.gap0" ".rodata.gap0" ".rodata.gap0" far_data
#pragma section sec_020cf078 begin
extern char data_ov007_020d7e00[];
extern char data_ov007_020d7e08[];
extern char data_ov007_020d7e10[];
BTAMaterialRecord data_ov007_020cf078[3] = {
    /* "nose" */
    {0xffff, 0, data_ov007_020d7e00, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1},
    /* "eye_L" */
    {0xffff, 0, data_ov007_020d7e08, 1, 1, 1, 1, 1, 0, 1, 2, 1, 3},
    /* "eye_R" */
    {0xffff, 0, data_ov007_020d7e10, 1, 1, 1, 1, 1, 0, 1, 4, 1, 5},
};
#pragma section sec_020cf078 end
