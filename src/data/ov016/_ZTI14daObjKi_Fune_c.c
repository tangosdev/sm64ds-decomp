/* ov016 cartridge data at 0x021149dc, 12 bytes. */
#include "types.h"
#include "rtti_abi.h"

extern u32 _ZTI10dBgActor_c[];
extern u32 _ZTS14daObjKi_Fune_c[];

#pragma define_section _ZTI14daObjKi_Fune_c ".data.morayTail" ".data.morayTail" ".data.morayTail" far_data
#pragma section _ZTI14daObjKi_Fune_c begin
u32 _ZTI14daObjKi_Fune_c[3] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE, (u32)&_ZTS14daObjKi_Fune_c, (u32)&_ZTI10dBgActor_c,
};
#pragma section _ZTI14daObjKi_Fune_c end
