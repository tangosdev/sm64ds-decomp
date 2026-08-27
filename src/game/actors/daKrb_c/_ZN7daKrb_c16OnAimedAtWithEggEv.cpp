//cpp
// @symbol _ZN7daKrb_c16OnAimedAtWithEggEv
/* recovered: real C++ method, vtable slot 29 -- overrides dActor_c::OnAimedAtWithEgg.
 * Previously func_ov084_0212b30c, blocked from naming because daKrb_c was a flat
 * struct with no base to diff its vtable against; see include/daKrb_c.h. Body
 * unchanged from the recovered free function -- only the symbol and self access
 * moved to a real method. */
#include "daKrb_c.h"

int daKrb_c::OnAimedAtWithEgg()
{
    unsigned short v = *(unsigned short*)((char*)this + 0xc);
    int b = (v == (unsigned short)0xc8) ? 1 : 0;
    if (b) return 0x41000;
    int b2 = (v == (unsigned short)0xca) ? 1 : 0;
    if (b2) return 0x96000;
    return 0x14000;
}
