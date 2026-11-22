/* claimed-storage */
/* ov070 .data.brqPmf 0x0212320c..0x0212323c. 6 storage addresses, one object. */
#include "types.h"

extern void _ZN7daBrq_c18EnterCooldownStateEv(void);
extern void _ZN7daBrq_c16EnterActiveStateEv(void);
extern void _ZN7daBrq_c18EnterDefeatedStateEv(void);
extern void _ZN7daBrq_c19UpdateCooldownStateEv(void);
extern void _ZN7daBrq_c19UpdateDefeatedStateEv(void);
extern void _ZN7daBrq_c17UpdateActiveStateEv(void);

#pragma define_section claimsec ".data.brqPmf" ".data.brqPmf" ".data.brqPmf" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov070_0212320c[] = {
    (u32)&_ZN7daBrq_c18EnterCooldownStateEv,
    0x00000000,
    (u32)&_ZN7daBrq_c16EnterActiveStateEv,
    0x00000000,
    (u32)&_ZN7daBrq_c18EnterDefeatedStateEv,
    0x00000000,
    (u32)&_ZN7daBrq_c19UpdateCooldownStateEv,
    0x00000000,
    (u32)&_ZN7daBrq_c19UpdateDefeatedStateEv,
    0x00000000,
    (u32)&_ZN7daBrq_c17UpdateActiveStateEv,
    0x00000000,
};
#pragma explicit_zero_data off
#pragma section claimsec end
