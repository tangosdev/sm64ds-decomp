//cpp
// @symbol daObjPathLift_c_classInit
#include "daObjPathLift_c.h"

extern "C" {
extern void _ZN10dBgActor_cC2Ev(dBgActor_c *actor);
extern void _ZN5ModelC1Ev(Model *model);
extern void _ZN5ModelD1Ev(Model *model);
extern void _ZN7PathPtrC1Ev(PathPtr *path);
extern void _ZN11ShadowModelC1Ev(ShadowModel *model);
extern void __cxa_vec_ctor(
    Model *models, int count, int size, void *ctor, void *dtor);
}

extern int data_ov002_0210af70[];
extern int _ZTV15daObjPathLift_c[];

/* Natural `return new daObjPathLift_c()` reproduces the text only when the
 * factory shares this class's TU, but Vector3's vague-linkage D1 then lands
 * between InitResources and classInit. Keep the typed C-ABI factory seam.
 * Historical alias: PathLift_Spawn. */
extern "C" daObjPathLift_c *daObjPathLift_c_classInit()
{
    daObjPathLift_c *actor =
        (daObjPathLift_c *)_ZN7fBase_cnwEj(sizeof(daObjPathLift_c));
    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *(int *)actor = (int)data_ov002_0210af70;
        __cxa_vec_ctor(
            actor->mModels, 3, sizeof(Model),
            (void *)_ZN5ModelC1Ev, (void *)_ZN5ModelD1Ev);
        _ZN7PathPtrC1Ev(&actor->mPath);
        *(int *)actor = (int)&_ZTV15daObjPathLift_c[2];
        _ZN11ShadowModelC1Ev(&actor->mShadowModel);
    }
    return actor;
}
