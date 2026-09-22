// @symbol daTrsTrap_c_classInit_TERESAPIT
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
/* Reconstructed source-style name: SM64DS proves daTrsTrap_c through RTTI,
 * allocation size, vtable identity, and the TERESAPIT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: TrapDoor_Spawn. */
int *daTrsTrap_c_classInit_TERESAPIT(void)
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
