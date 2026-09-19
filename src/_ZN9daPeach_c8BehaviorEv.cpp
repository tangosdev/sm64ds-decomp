//cpp
#include "Animation.h"
#include "dCc_c.h"
// @symbol _ZN9daPeach_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "daPeach_c.h"
extern "C" {
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); };

int daPeach_c::Behavior()
{
    CallStateBehavior();
    UpdateLookAt();
    if (mStateValue != 1)
        ((Animation *)((Animation *)&mModelAnim))->Advance();
    ((Sub*)((char *)&mModelAnim))->g3();
    ((dCc_c *)&mCylinder)->Clear();
    ((dCc_c *)&mCylinder)->Update();
    UpdateModel();
    return 1;
}
