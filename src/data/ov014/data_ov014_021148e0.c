/* ov014 cartridge data at 0x021148e0, 16 bytes. */
#include "types.h"

extern void _ZN7fBase_c13OnHeapCreatedEv(void);
extern void _ZN7fBase_c16OnPendingDestroyEv(void);
extern void _ZN7fBase_c9Virtual34Ejj(void);
extern void _ZN7fBase_c9Virtual38Ejj(void);

#pragma define_section data_ov014_021148e0 ".data.gap3" ".data.gap3" ".data.gap3" far_data
#pragma section data_ov014_021148e0 begin
u32 data_ov014_021148e0[4] = {
    (u32)&_ZN7fBase_c16OnPendingDestroyEv, (u32)&_ZN7fBase_c9Virtual34Ejj, (u32)&_ZN7fBase_c9Virtual38Ejj, (u32)&_ZN7fBase_c13OnHeapCreatedEv,
};
#pragma section data_ov014_021148e0 end
