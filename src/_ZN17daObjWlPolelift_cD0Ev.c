// @symbol _ZN17daObjWlPolelift_cD0Ev
/* daObjWlPolelift_c::~daObjWlPolelift_c (D0/deleting) */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int data_ov026_02113ae0[];
extern void _ZN10dCcAcPos_cD1Ev(void *);
extern void *data_020a0eac;
int *_ZN17daObjWlPolelift_cD0Ev(int *t)
{
    t[0] = (int)data_ov026_02113ae0;
    _ZN11ShadowModelD1Ev((char *)t + 0x188);
    _ZN10dCcAcPos_cD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
