/*
 * One object for data_ov006_0213c6bc, pmf_dScMg3DEsp_c_SlotDampen,
 * pmf_dScMg3DEsp_c_Wait (24 bytes): the linker starts every input section on
 * a 4-byte boundary, so these tables cannot be separate.
 */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
extern void _ZN12dScMg3DEsp_c4WaitEv(void *);
extern void _ZN12dScMg3DEsp_c10SlotDampenEi(void *, int);
u32 data_ov006_0213c6bc[6] = {
    0x6f6d6168, 0x0000006e, (u32)&_ZN12dScMg3DEsp_c10SlotDampenEi, 0x00000000, (u32)&_ZN12dScMg3DEsp_c4WaitEv, 0x00000000,
};
#pragma section obj end
