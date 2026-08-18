// @symbol _ZN13daObjMaruta_cD0Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV13daObjMaruta_c[];
extern int _ZTV10dBgActor_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjMaruta_c */
extern void *data_020a0eac;
/* daObjMaruta_c::~daObjMaruta_c() [D0, deleting], slot 17. Same reasoning
   as the D1 file above: the class's own destructor stays inline, but the
   vtable still needs a real deleting-destructor entry, spelled directly
   under its mangled name. */
int *_ZN13daObjMaruta_cD0Ev(int *t)
{
    t[0] = (int)_ZTV13daObjMaruta_c;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
