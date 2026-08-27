//cpp
// @symbol _ZN5Pokey8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Pokey.h"
extern "C" {
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void *c, int d);
extern void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(void *c, void *cyl);
}

int Pokey::Behavior()
{
    int s = mState;
    if (s != 2 && s != 5) {
        if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((char *)this), 0x5dc000)) return 1;
    }
    func_ov096_021368b4(((char *)this));
    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char *)this), ((char *)this) + 0x14c);
    func_ov096_02135efc(((char *)this));
    return 1;
}
