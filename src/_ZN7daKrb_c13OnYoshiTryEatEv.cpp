//cpp
// @symbol _ZN6Goomba13OnYoshiTryEatEv
/* recovered: real C++ method, vtable slot 18 -- overrides dActor_c::OnYoshiTryEat.
 * Previously func_ov084_0212bfc0, blocked from naming because Goomba was a flat
 * struct with no base to diff its vtable against; see include/Goomba.h. Body
 * unchanged from the recovered free function -- only the symbol and self access
 * moved to a real method. */
#include "Goomba.h"

int Goomba::OnYoshiTryEat()
{
    unsigned short v = *(unsigned short*)((char*)this + 0xc);
    int b = (v == (unsigned short)0xc8) ? 1 : 0;
    if (b) return 0x6;
    int b2 = (v == (unsigned short)0xc9) ? 1 : 0;
    if (b2) return 0x4;
    return 0x0;
}
