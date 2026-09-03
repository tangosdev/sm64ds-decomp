//cpp
// @symbol _ZN9daRFlag_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Timer.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daRFlag_c.h"
extern "C" {
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e);
}
extern char data_0209d4c8[];

int daRFlag_c::Behavior()
{
    unsigned int id;
    dActor_c *a;
    int b;

    if (mHasTouchedFlag == 0) {
        id = mdCcAc_c.otherOwner;
        if (id != 0) {
            a = dActor_c::FindWithID(id);
            if (a != 0) {
                b = a->actorID;
                b = b == 0xBF;
                if (b) {
                    mHasTouchedFlag = 1;
                    _ZN5Timer9StopTimerEv(data_0209d4c8);
                    mVictoryTimer = 1;
                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1F, 0x14, 0x7F, 0x6B000, 0);
                }
            }
        }
    }

    if (mVictoryTimer != 0) {
        *(unsigned short *)(((int)((char *)this) + 0x16C)) += 1;
        if (mVictoryTimer >= 0x5A) {
            if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1F, 0x7F, 0, 0x8777, 0) != 0) {
                mVictoryTimer = 0;
            }
        }
    }

    mModelAnim.Advance();
    func_ov062_0211afbc(((char *)this));
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}
