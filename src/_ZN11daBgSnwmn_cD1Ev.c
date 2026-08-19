// @symbol _ZN11daBgSnwmn_cD1Ev
/* daBgSnwmn_c::~daBgSnwmn_c() (complete-object / D1) -- vtable slots 16/17
 * (destructor pair, fBase_c/dActor_c convention). Tears down the five typed
 * sub-objects in reverse declaration order (mCylClsn, mShadow, mTexSeq,
 * mModel2, mModel1), then chains into dActor_c's D2. Plain C function
 * carrying the literal mangled name -- see include/daBgSnwmn_c.h. */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int data_ov072_02122978[];
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN15TextureSequenceD1Ev(void *);
int *_ZN11daBgSnwmn_cD1Ev(int *t)
{
    t[0] = (int)data_ov072_02122978;
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x1b0);
    _ZN11ShadowModelD1Ev((char *)t + 0x188);
    _ZN15TextureSequenceD1Ev((char *)t + 0x174);
    _ZN5ModelD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}
