/* ov016 cartridge data at 0x02114b80, 12 bytes. */
#include "types.h"

extern u32 _ZTI17daObjFloatBoard_c[];
extern u32 _ZTS13daObjKi_Ita_c[];
extern u32 _ZTVN3abi20__si_class_type_infoE[];

#pragma define_section _ZTI13daObjKi_Ita_c ".data.morayTail" ".data.morayTail" ".data.morayTail" far_data
#pragma section _ZTI13daObjKi_Ita_c begin
u32 _ZTI13daObjKi_Ita_c[3] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE, (u32)&_ZTS13daObjKi_Ita_c, (u32)&_ZTI17daObjFloatBoard_c,
};
#pragma section _ZTI13daObjKi_Ita_c end
