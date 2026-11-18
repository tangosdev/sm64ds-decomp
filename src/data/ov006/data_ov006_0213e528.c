/*
 * One object for data_ov006_0213e528, pmf_Slot3_StateSpin,
 * pmf_Slot3_StatePayout (24 bytes): the linker starts every input section on
 * a 4-byte boundary, so these tables cannot be separate.
 */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
extern u8 data_ov006_0213e540[];
extern struct OamAttr data_ov006_0213e500;
extern void _ZN12dScMgSlot3_c9StateSpinEv(void *);
extern void _ZN12dScMgSlot3_c11StatePayoutEv(void *);
void * data_ov006_0213e528[6] = {
    (void *)&data_ov006_0213e500, (void *)&data_ov006_0213e540, (void *)&_ZN12dScMgSlot3_c9StateSpinEv, (void *)0x00000000, (void *)&_ZN12dScMgSlot3_c11StatePayoutEv, (void *)0x00000000,
};
#pragma section obj end
