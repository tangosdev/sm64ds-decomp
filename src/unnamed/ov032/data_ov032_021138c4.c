/* ov032 cartridge data at 0x021138c4, 28 bytes.
 * The last two words are data_ov032_021138d8: offset-to-top, then the
 * pointer to _ZTI13daObjTdFuta_c. They share this object so the relocated
 * word is not emitted in a separate .data section ahead of these words. */
#include "types.h"

extern u32 _ZTI13daObjTdFuta_c[];

u32 data_ov032_021138c4[7] = {
    0x00000006,
    0x00064000,
    0x000fa000,
    0x00c80000,
    0x00640000,
    0x00000000,
    (u32)&_ZTI13daObjTdFuta_c,
};
