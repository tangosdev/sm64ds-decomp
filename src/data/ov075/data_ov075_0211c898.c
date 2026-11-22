/* claimed-storage */
/* ov075 .data.entry_sprite_gap7 0x0211c898..0x0211c8e0. 9 storage addresses, one object. */
#include "types.h"

extern void _ZN10dScEntry_c18UpdateCourseSelectEv(void);
extern void _ZN10dScEntry_c16EnterQuitMessageEv(void);
extern void _ZN10dScEntry_c17EnterMatchLoadingEv(void);
extern void _ZN10dScEntry_c15UpdateSoloStartEv(void);
extern void _ZN10dScEntry_c17EnterSessionStartEv(void);
extern void _ZN10dScEntry_c17RenderRecruitTextEv(void);
extern void _ZN10dScEntry_c15UpdateStepDelayEv(void);
extern void _ZN10dScEntry_c18UpdateSessionStartEv(void);

#pragma define_section claimsec ".data.entry_sprite_gap7" ".data.entry_sprite_gap7" ".data.entry_sprite_gap7" far_data
#pragma section claimsec begin
u32 data_ov075_0211c898[] = {
    (u32)&_ZN10dScEntry_c18UpdateCourseSelectEv,
    0x00000000,
    (u32)&_ZN10dScEntry_c16EnterQuitMessageEv,
    0x00000000,
    (u32)&_ZN10dScEntry_c17EnterMatchLoadingEv,
    0x00000000, 0x01f480f8, 0xffff14c0,
    (u32)&_ZN10dScEntry_c15UpdateSoloStartEv,
    0x00000000,
    (u32)&_ZN10dScEntry_c17EnterSessionStartEv,
    0x00000000,
    (u32)&_ZN10dScEntry_c17RenderRecruitTextEv,
    0x00000000,
    (u32)&_ZN10dScEntry_c15UpdateStepDelayEv,
    0x00000000,
    (u32)&_ZN10dScEntry_c18UpdateSessionStartEv,
    0x00000000,
};
#pragma section claimsec end
