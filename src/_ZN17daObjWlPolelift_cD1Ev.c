// @symbol _ZN17daObjWlPolelift_cD1Ev
/* daObjWlPolelift_c::~daObjWlPolelift_c (D1/complete) */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int data_ov026_02113ae0[];
extern void _ZN10dCcAcPos_cD1Ev(void *);
int *_ZN17daObjWlPolelift_cD1Ev(int *t)
{
    t[0] = (int)data_ov026_02113ae0;
    _ZN11ShadowModelD1Ev((char *)t + 0x188);
    _ZN10dCcAcPos_cD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}
