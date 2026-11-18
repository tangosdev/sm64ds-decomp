/* ov043 cartridge data at 0x021123e4, 16 bytes. */
#include "types.h"

extern void _ZN8dActor_c11OnAttacked1ERS_(void);
extern void _ZN8dActor_c11OnAttacked2ERS_(void);
extern void _ZN8dActor_c8OnKickedERS_(void);
extern void _ZN8dActor_c8OnPushedERS_(void);

u32 data_ov043_021123e4[4] = {
    (u32)&_ZN8dActor_c11OnAttacked1ERS_, (u32)&_ZN8dActor_c11OnAttacked2ERS_, (u32)&_ZN8dActor_c8OnKickedERS_, (u32)&_ZN8dActor_c8OnPushedERS_,
};
