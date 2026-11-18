/*
 * One object for data_ov006_0213add8, pmf_Presenter_StateWin (16 bytes): the
 * linker starts every input section on a 4-byte boundary, so these tables
 * cannot be separate.
 */
#include "types.h"

#pragma define_section obj ".data.gap45" ".data.gap45" ".data.gap45" far_data
#pragma section obj begin
extern void _ZN9Presenter8StateWinEv(void *);
u32 data_ov006_0213add8[4] = {
    0x79655f6d, 0x00725f65, (u32)&_ZN9Presenter8StateWinEv, 0x00000000,
};
#pragma section obj end
