//cpp
#include "dScMgTeresa_c.h"
// @symbol _ZN13dScMgTeresa_c9Virtual50Ev
/* Minigame slot 20; Virtual50 is a placeholder, not an original name.
   The reconstructed void contract is documented in dScMgBase_c.h. */
/* _ZN13dScMgTeresa_c9Virtual50Ev at 0x02120238 - thunk: FreeGfxSlotsById(8) */

extern "C" {
extern void FreeGfxSlotsById(int a);
}

void dScMgTeresa_c::Virtual50()
{
    FreeGfxSlotsById(8);
}
