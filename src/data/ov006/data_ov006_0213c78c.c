/*
 * One object for g_profile_MG_3DESP, pmf_dScMg3DEsp_c_Resolve,
 * pmf_dScMg3DEsp_c_Await, pmf_dScMg3DEsp_c_SlotWait, pmf_dScMg3DEsp_c_Deal,
 * pmf_dScMg3DEsp_c_Results (48 bytes): the linker starts every input section
 * on a 4-byte boundary, so these tables cannot be separate.
 */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
extern void _ZN12dScMg3DEsp_c8SlotWaitEi(void *, int);
extern void _ZN12dScMg3DEsp_c7ResultsEv(void *);
extern void _ZN12dScMg3DEsp_c7ResolveEv(void *);
extern void _ZN12dScMg3DEsp_c5AwaitEv(void *);
extern void _ZN12dScMg3DEsp_c4DealEv(void *);
extern struct dScMg3DEsp_c * _Z22dScMg3DEsp_c_classInitv();
u32 g_profile_MG_3DESP[12] = {
    (u32)&_Z22dScMg3DEsp_c_classInitv, 0x01850185, (u32)&_ZN12dScMg3DEsp_c7ResolveEv, 0x00000000, (u32)&_ZN12dScMg3DEsp_c5AwaitEv, 0x00000000, (u32)&_ZN12dScMg3DEsp_c8SlotWaitEi, 0x00000000,
    (u32)&_ZN12dScMg3DEsp_c4DealEv, 0x00000000, (u32)&_ZN12dScMg3DEsp_c7ResultsEv, 0x00000000,
};
#pragma section obj end
