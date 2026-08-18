// @symbol _ZN13daObjMaruta_cD1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV13daObjMaruta_c[];
extern int _ZTV10dBgActor_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjMaruta_c */
/* daObjMaruta_c::~daObjMaruta_c() [D1, base object], slot 16. The class
   keeps its destructor declared inline in daObjMaruta_c.h so every
   descendant inlines this body rather than emitting a `bl` here -- but
   the compiler still needs one out-of-line copy for the vtable slot
   itself, which is what this file spells directly under the destructor's
   real mangled name rather than a func_ov080_ placeholder. */
int *_ZN13daObjMaruta_cD1Ev(int *t)
{
    t[0] = (int)_ZTV13daObjMaruta_c;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}
