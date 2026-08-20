// @symbol _ZN11daBgSnwmn_cD0Ev
/* daBgSnwmn_c::~daBgSnwmn_c() (deleting / D0) -- vtable slots 16/17. Same
 * teardown as D1, then deallocates. Plain C function carrying the literal
 * mangled name -- see include/daBgSnwmn_c.h. */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int data_ov072_02122978[];
extern void _ZN10dCcAcPos_cD1Ev(void *);
extern void _ZN15TextureSequenceD1Ev(void *);
extern void *data_020a0eac;
int *_ZN11daBgSnwmn_cD0Ev(int *t)
{
    t[0] = (int)data_ov072_02122978;
    _ZN10dCcAcPos_cD1Ev((char *)t + 0x1b0);
    _ZN11ShadowModelD1Ev((char *)t + 0x188);
    _ZN15TextureSequenceD1Ev((char *)t + 0x174);
    _ZN5ModelD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
