/* ov016 cartridge data at 0x02114c68, 36 bytes. */
#include "types.h"

struct daSlide_Box_c;
extern struct daSlide_Box_c *_Z23daSlide_Box_c_classInitv(void);
extern u32 _ZTI13daSlide_Box_c[];

#pragma define_section g_profile_SLIDE_BOX ".data.morayTail"                                       \
                                           ".data.morayTail"                                       \
                                           ".data.morayTail" far_data
#pragma section g_profile_SLIDE_BOX begin
u32 g_profile_SLIDE_BOX[9] = {
    (u32)&_Z23daSlide_Box_c_classInitv,
    0x01380139,
    0x00000002,
    0x00096000,
    0x0012c000,
    0x02000000,
    0x02000000,
    0x00000000,
    (u32)&_ZTI13daSlide_Box_c,
};
#pragma section g_profile_SLIDE_BOX end
