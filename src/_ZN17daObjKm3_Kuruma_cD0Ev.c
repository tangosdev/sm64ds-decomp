// @symbol _ZN17daObjKm3_Kuruma_cD0Ev
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV17daObjKm3_Kuruma_c[];
extern int _ZTV13daObjKuruma_c[];
extern int _ZTV10dBgActor_c[];
/* recovered: vtable identified, renamed to Class_Method */
extern void *data_020a0eac;
int *_ZN17daObjKm3_Kuruma_cD0Ev(int *t)
{
    t[0] = (int)_ZTV17daObjKm3_Kuruma_c;
    t[0] = (int)_ZTV13daObjKuruma_c;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
