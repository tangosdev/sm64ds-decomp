/* claimed-storage */
/* ov004 .data.gap23 0x020bbf6c..0x020bbf94. 3 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS11dScMgBase_c[];
extern u32 _ZTI8dScene_c[];
extern u32 _ZTSN11dScMgBase_c15graphCallback_cE[];
extern u32 _ZTIN8dGraph_c10callback_cE[];

#pragma define_section claimsec ".data.gap23" ".data.gap23" ".data.gap23" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTI11dScMgBase_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS11dScMgBase_c,
    (u32)&_ZTI8dScene_c,
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTSN11dScMgBase_c15graphCallback_cE,
    (u32)&_ZTIN8dGraph_c10callback_cE,
    0x53643131, 0x42674d63, 0x5f657361, 0x00000063,
};
#pragma explicit_zero_data off
#pragma section claimsec end
