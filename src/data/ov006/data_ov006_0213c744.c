/*
 * One object for pmf_dMg3DEspModel_c_Idle_2, pmf_dScMg3DEsp_c_SlotRise,
 * pmf_dMg3DEspModel_c_Hit (24 bytes): the linker starts every input section
 * on a 4-byte boundary, so these tables cannot be separate.
 */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
extern void _ZN15dMg3DEspModel_c4IdleEv(void *);
extern void _ZN15dMg3DEspModel_c3HitEv(void *);
extern void _ZN12dScMg3DEsp_c8SlotRiseEi(void *, int);
int pmf_dMg3DEspModel_c_Idle_2[6] = {
    (int)&_ZN15dMg3DEspModel_c4IdleEv, 0x00000000, (int)&_ZN12dScMg3DEsp_c8SlotRiseEi, 0x00000000, (int)&_ZN15dMg3DEspModel_c3HitEv, 0x00000000,
};
#pragma section obj end
