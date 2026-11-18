/*
 * One object for sCardHandPayouts, sCardFaceSprites (108 bytes): the linker
 * starts every input section on a 4-byte boundary, so these tables cannot be
 * separate.
 */
#include "types.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
extern u8 _ZTI11dScMgCard_c[];
int sCardHandPayouts[27] = {
    0x00000000, 0x00000002, 0x00000003, 0x00000004, 0x00000006, 0x00000008, 0x00000010, 0x00000000,
    0x00000000, 0x00000000, 0x00020001, 0x00030004, 0x00010000, 0x00060002, 0x00000005, 0x00020001,
    0x00070008, 0x00010000, 0x000a0002, 0x00000009, 0x00020001, 0x000b000c, 0x00010000, 0x000e0002,
    0x0000000d, 0x00000000, (int)&_ZTI11dScMgCard_c,
};
#pragma section obj end
