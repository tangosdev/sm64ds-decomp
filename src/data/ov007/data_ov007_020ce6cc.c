/* 0x020ce6cc: BTA material records of data_ov007_020cd9b4.
 * 2 BTAMaterialRecord rows, const (rodata). */
#include "types.h"
#include "private/BTA_File.h"

#pragma define_section sec_020ce6cc ".rodata.gap21" ".rodata.gap21" ".rodata.gap21" far_data
#pragma section sec_020ce6cc begin
extern char data_ov007_020d7e18[];
extern char data_ov007_020d7e20[];
BTAMaterialRecord data_ov007_020ce6cc[2] = {
    /* "m_eye_l" */
    {0xffff, 0, data_ov007_020d7e18, 1, 0, 1, 0, 1, 0, 48, 0, 48, 48},
    /* "m_eye_r" */
    {0xffff, 0, data_ov007_020d7e20, 1, 0, 1, 0, 1, 1, 48, 96, 48, 144},
};
#pragma section sec_020ce6cc end
