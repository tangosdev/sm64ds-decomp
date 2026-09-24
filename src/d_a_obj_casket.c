#include "daObjCasket_c.h"
// @symbol daObjCasket_c_classInit
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV13daObjCasket_c */
/* Reconstructed source-style name: SM64DS proves daObjCasket_c through RTTI,
 * allocation size, vtable identity, and the CASKET registry profile; later EAD
 * lineage supplies classInit. Exact original spelling is not preserved. The
 * tree called the class Coffin before the RTTI rename. Historical alias:
 * Coffin_Spawn. */
extern int _ZTV13daObjCasket_c[];
int *daObjCasket_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(sizeof(struct daObjCasket_c));
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV13daObjCasket_c; }
    return p;
}
