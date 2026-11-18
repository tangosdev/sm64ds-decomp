/* ov016 cartridge data at 0x02114c4c, 12 bytes. */
#include "types.h"
#include "rtti_abi.h"

extern u32 _ZTI10dBgActor_c[];
extern char _ZTS13daSlide_Box_c[];

#pragma define_section _ZTI13daSlide_Box_c ".data.morayTail"                                       \
                                           ".data.morayTail"                                       \
                                           ".data.morayTail" far_data
#pragma section _ZTI13daSlide_Box_c begin
u32 _ZTI13daSlide_Box_c[3] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS13daSlide_Box_c,
    (u32)&_ZTI10dBgActor_c,
};
#pragma section _ZTI13daSlide_Box_c end
