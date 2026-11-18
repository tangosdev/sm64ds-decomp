/* ov032 cartridge data at 0x021139f4, 12 bytes. */
#include "types.h"

extern void _ZN8dActor_c11OnAttacked1ERS_(void);
extern void _ZN8dActor_c15OnGroundPoundedERS_(void);
extern void _ZN8dActor_c9Virtual50Ev(void);

u32 data_ov032_021139f4[3] = {
    (u32)&_ZN8dActor_c9Virtual50Ev, (u32)&_ZN8dActor_c15OnGroundPoundedERS_, (u32)&_ZN8dActor_c11OnAttacked1ERS_,
};
