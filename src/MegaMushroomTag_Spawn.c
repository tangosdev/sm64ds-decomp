// @symbol MegaMushroomTag_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV21MegaMushroomCreateTag */
/* A real-C++ `new MegaMushroomCreateTag` probe reproduces the instruction
 * shape, but CW names the allocation relocation `_Znwm` (global operator new).
 * With that unresolved name mapped to the ROM's only global operator new, the
 * relocation-aware check is WRONG with blind:0 at +0x08: this factory calls
 * fBase_c::operator new instead. Keep this ABI boundary in C. */
int *MegaMushroomTag_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(272);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV21MegaMushroomCreateTag;
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}
