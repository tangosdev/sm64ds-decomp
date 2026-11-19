/* ov029 cartridge data at 0x02113cd4, 28 bytes.
 * The last word is data_ov029_02113cec, the typeinfo pointer immediately
 * before _ZTV15daObjWc_Obj02_c. It has to share this object: a relocated
 * word and a pure word land in different .data sections, and the link
 * would concatenate those sections in the wrong order. */
#include "types.h"

extern u32 _ZTI15daObjWc_Obj02_c[];

u32 data_ov029_02113cd4[7] = {
    0x00000002,
    0x00000000,
    0x00100000,
    0x01000000,
    0x01000000,
    0x00000000,
    (u32)&_ZTI15daObjWc_Obj02_c,
};
