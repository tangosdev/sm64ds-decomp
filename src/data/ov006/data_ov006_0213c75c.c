/*
 * One object for data_ov006_0213c75c, pmf_dMg3DEspModel_c_Intro,
 * pmf_dMg3DEspModel_c_Wait, pmf_dMg3DEspModel_c_Idle_3,
 * pmf_dScMg3DEsp_c_CardTouch, pmf_dScMg3DEsp_c_FlashQuick (48 bytes): the
 * linker starts every input section on a 4-byte boundary, so these tables
 * cannot be separate.
 */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
extern void _ZN15dMg3DEspModel_c5IntroEv(void *);
extern void _ZN15dMg3DEspModel_c4WaitEv(void *);
extern void _ZN15dMg3DEspModel_c4IdleEv(void *);
extern void _ZN12dScMg3DEsp_c9CardTouchEi(void *, int);
extern void _ZN12dScMg3DEsp_c10FlashQuickEv(void *);
u32 data_ov006_0213c75c[12] = {
    0x6d5f696b, 0x00757a69, (u32)&_ZN15dMg3DEspModel_c5IntroEv, 0x00000000, (u32)&_ZN15dMg3DEspModel_c4WaitEv, 0x00000000, (u32)&_ZN15dMg3DEspModel_c4IdleEv, 0x00000000,
    (u32)&_ZN12dScMg3DEsp_c9CardTouchEi, 0x00000000, (u32)&_ZN12dScMg3DEsp_c10FlashQuickEv, 0x00000000,
};
#pragma section obj end
