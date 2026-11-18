/* 0x020ce8c4: BTA material records of data_ov007_020cdb04.
 * 2 BTAMaterialRecord rows, const (rodata). */
#include "types.h"
#include "private/BTA_File.h"

#pragma define_section sec_020ce8c4 ".rodata.gap24" ".rodata.gap24" ".rodata.gap24" far_data
#pragma section sec_020ce8c4 begin
extern char data_ov007_020d7e18[];
extern char data_ov007_020d7e20[];
BTAMaterialRecord data_ov007_020ce8c4[2] = {
    /* "m_eye_l" */
    {0xffff, 0, data_ov007_020d7e18, 1, 0, 1, 0, 1, 0, 700, 0, 700, 700},
    /* "m_eye_r" */
    {0xffff, 0, data_ov007_020d7e20, 1, 0, 1, 0, 1, 0, 700, 1400, 700, 2100},
};
#pragma section sec_020ce8c4 end
