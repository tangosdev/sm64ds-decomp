/* 0x020ce854: BTA material records of data_ov007_020cdabc.
 * 2 BTAMaterialRecord rows, const (rodata). */
#include "types.h"
#include "private/BTA_File.h"

#pragma define_section sec_020ce854 ".rodata.gap23" ".rodata.gap23" ".rodata.gap23" far_data
#pragma section sec_020ce854 begin
extern char data_ov007_020d7e18[];
extern char data_ov007_020d7e20[];
BTAMaterialRecord data_ov007_020ce854[2] = {
    /* "m_eye_l" */
    {0xffff, 0, data_ov007_020d7e18, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    /* "m_eye_r" */
    {0xffff, 0, data_ov007_020d7e20, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
};
#pragma section sec_020ce854 end
