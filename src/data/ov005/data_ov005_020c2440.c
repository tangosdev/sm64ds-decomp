/* claimed-storage */
/* ov005 .data.early 0x020c2440..0x020c2464. 3 storage addresses, one object. */
#include "types.h"

extern void _Z21dScMiniGm_c_classInitv(void);
extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS11dScMiniGm_c[];
extern u32 _ZTI8dScene_c[];

#pragma define_section claimsec ".data.early" ".data.early" ".data.early" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 g_profile_MINIGAME[] = {
    (u32)&_Z21dScMiniGm_c_classInitv,
    0x00080005,
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS11dScMiniGm_c,
    (u32)&_ZTI8dScene_c,
    0x53643131, 0x6e694d63, 0x5f6d4769, 0x00000063,
};
#pragma explicit_zero_data off
#pragma section claimsec end
