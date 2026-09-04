// @symbol daObjNumber_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_TextureSequence.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV6Number */
/* Reconstructed source-style name: SM64DS proves daObjNumber_c through RTTI,
 * allocation size, vtable identity, and the OBJ_NUMBER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Number_Spawn. */
int *daObjNumber_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(336);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV6Number;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN15TextureSequenceC1Ev((char *)p + 0x124);
    }
    return p;
}
