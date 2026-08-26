// @symbol TreasureChest_Spawn
/* Measured C++ factory wall. `return new TreasureChest` reproduces all 0x40
 * bytes with mwccarm 2004/b56, including the base/member construction order,
 * but emits an unresolved `_Znwm` relocation at +0x08. The ROM calls
 * fBase_c::operator new(unsigned int) there. Retain the C ABI construction
 * form until that allocator relationship is representable without a wrong or
 * blind relocation. */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"

int *TreasureChest_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x178);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV13TreasureChest;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN7dCcAc_cC1Ev((char *)p + 0x138);
    }
    return p;
}
