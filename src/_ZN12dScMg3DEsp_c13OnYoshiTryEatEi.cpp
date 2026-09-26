//cpp
// @symbol _ZN12dScMg3DEsp_c13OnYoshiTryEatEi
#include "decl_common.h"
#include "dMg3DEspModel_c.h"
#include "dScMg3DEsp_c.h"
/* dScMg3DEsp_c::OnYoshiTryEat, vtable slot 18. The name is a placeholder
   borrowed from dActor_c by slot index; see dScMgBase_c.h. The signature
   repeats the base declaration, or mwcc appends a slot instead of
   overriding. */

void dScMg3DEsp_c::OnYoshiTryEat(int a)
{
    func_ov006_020e984c((char *)this);
    unk_553c = 0;
    if (a == 0) {
        unk_0bc++;
        if (unk_0bc > 9998) unk_0bc = 9998;
    } else if (a == 0x12) {
        if (data_ov004_020beb68 != 0) ((dScMgBase_c*)data_ov004_020beb68)->mHudScore = 0;
        unk_0bc = 0;
        if (unk_0bc > 9998) unk_0bc = 9998; /* the ROM keeps this clamp; 16 bytes */
    }
    unk_51e4 = 0;
    ((dMg3DEspModel_c*)pad_4fd8)->Reset();
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
}
