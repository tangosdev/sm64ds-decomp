/*
 * One object for data_ov006_0213c6d4, pmf_dScMg3DEsp_c_FlashOnce,
 * pmf_dScMg3DEsp_c_RowWait, pmf_dScMg3DEsp_c_SlotKick,
 * pmf_dScMg3DEsp_c_SlotSetup, pmf_dScMg3DEsp_c_Play,
 * pmf_dMg3DEspModel_c_Idle, pmf_dScMg3DEsp_c_SlotFall,
 * pmf_dScMg3DEsp_c_CardSeek, pmf_dScMg3DEsp_c_RowDecay,
 * pmf_dScMg3DEsp_c_FlashSync, pmf_dScMg3DEsp_c_Fade,
 * pmf_dScMg3DEsp_c_CardExit, pmf_dScMg3DEsp_c_RowFade (112 bytes): the
 * linker starts every input section on a 4-byte boundary, so these tables
 * cannot be separate.
 */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
extern void _ZN15dMg3DEspModel_c4IdleEv(void *);
extern void _ZN12dScMg3DEsp_c9SlotSetupEi(void *, int);
extern void _ZN12dScMg3DEsp_c9FlashSyncEv(void *);
extern void _ZN12dScMg3DEsp_c9FlashOnceEv(void *);
extern void _ZN12dScMg3DEsp_c8SlotKickEi(void *, int);
extern void _ZN12dScMg3DEsp_c8SlotFallEi(void *, int);
extern void _ZN12dScMg3DEsp_c8RowDecayEi(void *, int);
extern void _ZN12dScMg3DEsp_c8CardSeekEi(void *, int);
extern void _ZN12dScMg3DEsp_c8CardExitEi(void *, int);
extern void _ZN12dScMg3DEsp_c7RowWaitEi(void *, int);
extern void _ZN12dScMg3DEsp_c7RowFadeEi(void *, int);
extern void _ZN12dScMg3DEsp_c4PlayEv(void *);
extern void _ZN12dScMg3DEsp_c4FadeEv(void *);
u32 data_ov006_0213c6d4[28] = {
    0x00600060, 0x006000a0, (u32)&_ZN12dScMg3DEsp_c9FlashOnceEv, 0x00000000, (u32)&_ZN12dScMg3DEsp_c7RowWaitEi, 0x00000000, (u32)&_ZN12dScMg3DEsp_c8SlotKickEi, 0x00000000,
    (u32)&_ZN12dScMg3DEsp_c9SlotSetupEi, 0x00000000, (u32)&_ZN12dScMg3DEsp_c4PlayEv, 0x00000000, (u32)&_ZN15dMg3DEspModel_c4IdleEv, 0x00000000, (u32)&_ZN12dScMg3DEsp_c8SlotFallEi, 0x00000000,
    (u32)&_ZN12dScMg3DEsp_c8CardSeekEi, 0x00000000, (u32)&_ZN12dScMg3DEsp_c8RowDecayEi, 0x00000000, (u32)&_ZN12dScMg3DEsp_c9FlashSyncEv, 0x00000000, (u32)&_ZN12dScMg3DEsp_c4FadeEv, 0x00000000,
    (u32)&_ZN12dScMg3DEsp_c8CardExitEi, 0x00000000, (u32)&_ZN12dScMg3DEsp_c7RowFadeEi, 0x00000000,
};
#pragma section obj end
