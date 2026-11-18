/* ov014 cartridge data at 0x021145b8, 12 bytes. */
#include "types.h"

extern u32 _ZTI13daObjSwdoor_c[];
extern u32 _ZTS14daObjBSwdoor_c[];
extern u32 _ZTVN3abi20__si_class_type_infoE[];

#pragma define_section _ZTI14daObjBSwdoor_c ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section _ZTI14daObjBSwdoor_c begin
u32 _ZTI14daObjBSwdoor_c[3] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE, (u32)&_ZTS14daObjBSwdoor_c, (u32)&_ZTI13daObjSwdoor_c,
};
#pragma section _ZTI14daObjBSwdoor_c end
