// @symbol MerryGoRound_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV11daTrsTrap_c */
/* KAIDAN, BOOKSHELF, MERRYGOROUND, and TERESAPIT all construct the ROM-proven
 * daTrsTrap_c class. A class-anchored classInit rename would collide, so this
 * factory keeps its historical project spelling. */
int *MerryGoRound_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(852);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV11daTrsTrap_c;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN10dBgW_KcMbgC1Ev((char *)p + 0x15c);
    }
    return p;
}
