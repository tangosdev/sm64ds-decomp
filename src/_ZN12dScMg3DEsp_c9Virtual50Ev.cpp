//cpp
#include "dScMg3DEsp_c.h"
// @symbol _ZN12dScMg3DEsp_c9Virtual50Ev
/* Minigame slot 20; Virtual50 is a placeholder, not an original name.
   The reconstructed void contract is documented in dScMgBase_c.h. */
/* _ZN12dScMg3DEsp_c9Virtual50Ev at 0x020e9c10 - thunk: FreeGfxSlotsById(8) */

extern "C" {
extern void FreeGfxSlotsById(int a);
}

void dScMg3DEsp_c::Virtual50()
{
    FreeGfxSlotsById(8);
}
