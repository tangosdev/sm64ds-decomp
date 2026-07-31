// @symbol _ZN15InvisibleSecretD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV13daObjNumber_c */
extern void _ZN15TextureSequenceD1Ev(void *);
extern void *G0;
int *_ZN15InvisibleSecretD0Ev(int *t)
{
    t[0] = (int)_ZTV13daObjNumber_c;
    _ZN15TextureSequenceD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
