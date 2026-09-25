// @symbol daObjEwmIceBlock_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV18daObjEwmIceBlock_c */
extern void _ZN7PathPtrC1Ev(void *);
/* Reconstructed source-style name: SM64DS proves daObjEwmIceBlock_c through RTTI,
 * allocation size, vtable identity, and the EWM_ICE_BLOCK registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: BigMovingIceBlock_Spawn.
 * Leftover: written as C, not `return new daObjEwmIceBlock_c`. The new-expression
 * matches this body but also emits a homeless _ZN10dBgActor_cD2Ev, which this
 * separate TU cannot deadstrip. This TU imports the vtable, so the store names
 * the slot array directly. */
int *daObjEwmIceBlock_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV18daObjEwmIceBlock_c;
        _ZN7PathPtrC1Ev((char *)p + 0x320);
    }
    return p;
}
