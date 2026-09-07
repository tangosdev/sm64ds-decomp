//cpp
// @symbol _ZN13PrincessPeach8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "PrincessPeach.h"
extern "C" {
extern void _ZN9Animation7AdvanceEv(void *a);
extern void _ZN5dCc_c5ClearEv(void *);
extern void _ZN5dCc_c6UpdateEv(void *);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); };

int PrincessPeach::Behavior()
{
    CallStateBehavior();
    UpdateLookAt();
    if (mStateValue != 1)
        _ZN9Animation7AdvanceEv((char *)(Animation *)&mModelAnim);
    ((Sub*)((char *)&mModelAnim))->g3();
    _ZN5dCc_c5ClearEv((char *)&mCylinder);
    _ZN5dCc_c6UpdateEv((char *)&mCylinder);
    UpdateModel();
    return 1;
}
